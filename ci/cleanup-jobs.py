import os
import sys
import gitlab
import datetime
import time
import tqdm


GITLAB_SERVER = os.environ.get('GL_SERVER', 'https://gitlab.lre.epita.fr')
GITLAB_TOKEN = os.environ.get('GL_TOKEN') # token requires developer permissions
PROJECT_ID = os.environ.get('GL_PROJECT_ID', 552)

MAX_JOBS = 100000

if not GITLAB_TOKEN:
        print("🤔 Please set the GL_TOKEN env variable.")
        sys.exit(1)

gl = gitlab.Gitlab(GITLAB_SERVER, private_token=GITLAB_TOKEN, pagination="keyset", order_by="id", per_page=100)


ci_job_artifacts = []
projects = [gl.projects.get(PROJECT_ID)]
jobs_marked_delete_artifacts = []

# Advanced filtering: Age and Size
# Example: 45 days, 10 MB threshold (TODO: Make this configurable)
threshold_age = 45 * 24 * 3600

for project in projects:
    project_obj = gl.projects.get(project.id)
    jobs = project.jobs.list(order_by="id", per_page=100, iterator=True)
    for i, job in tqdm.tqdm(enumerate(jobs), desc="Collecting jobs"):
        # strptime this time format: 2025-03-18T08:14:02.367+01:00
        created_at = datetime.datetime.strptime(job.created_at, "%Y-%m-%dT%H:%M:%S.%f%z").replace(tzinfo=None)
        now = datetime.datetime.now().replace(tzinfo=None)
        age = (now - created_at).total_seconds()
        if i % 50 == 0:
            print("ID:", job.id, "AGE:", int(age / 3600 / 24), "d")
        if age > threshold_age:
            jobs_marked_delete_artifacts.append(job)
        if len(jobs_marked_delete_artifacts) >= MAX_JOBS:
            break
print("===================")
print("Done collecting data:", len(jobs_marked_delete_artifacts), "jobs marked for deletion.")

# Advanced filtering: Delete all job artifacts marked to being deleted.
for job in jobs_marked_delete_artifacts:
    # delete the artifact
    #job.delete_artifacts()
    try:
        job.erase()
        print("Deleted job", job.id, job.name)
    except Exception as e:
        print("Failed to delete job", job.id, job.name, e)
    # Sleep for 500ms second
    time.sleep(0.5)
# Print collection summary (removed for readability)