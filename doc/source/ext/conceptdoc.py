from docutils import nodes
from docutils.parsers import rst
from sphinx.directives import ObjectDescription
from sphinx.domains import Domain, ObjType
from sphinx.roles import XRefRole
from sphinx.util.nodes import make_refnode
from sphinx.locale import l_, _

class concept(nodes.Element):
    pass


class ConceptDirective(ObjectDescription):
    has_content = True
    required_arguments = 2
    final_argument_whitespace = True
    option_spec = {
        "refines" : rst.directives.unchanged
    }

    def add_target_and_index(self, name, sig, signode):
        targetname = "concept-" + name
        if targetname not in self.state.document.ids:
            #signode['names'].append(targetname)
            #signode['ids'].append(targetname)
            #signode["first"] = (not self.names)
            #self.state.document.note_explicit_target(signode)

            objects = self.env.domaindata['concept']['objects']
            key = (self.objtype, name)
            if key in objects:
                self.env.warn(self.env.docname,
                              'duplicate description of %s %s, ' %
                              (self.objtype, name) +
                              'other instance in ' +
                              self.env.doc2path(objects[key]),
                              self.lineno)
            objects[key] = self.env.docname


    def handle_signature(self, sig, signode):
        targetname = "concept-" + self.arguments[0]
        mynode = nodes.section(ids=[targetname])
        title = nodes.title(text=self.arguments[1])
        mynode.append(title)
        signode += mynode
        return self.arguments[0]


    # def run(self):
    #     lnodes = []
    #     env = self.state.document.settings.env
    #     targetid = "concept:" + self.arguments[0]
    #     mynode = nodes.section(ids=[targetid])
    #     title = nodes.title(text=self.arguments[1])
    #     mynode.append(title)

    #     # Concept refinement
    #     if "refines" in self.options:
    #         print "opt"
    #         n = nodes.topic()
    #         n.append(nodes.title(text="Refines:"))
    #         n.append(nodes.paragraph())
    #         n[-1].append(nodes.reference(text=self.options["refines"], refid="concept:%s" % self.options["refines"]))
    #         mynode.append(n)
    #     return [mynode]

class ConceptDomain(Domain):
    """ Concept domain """
    name = "concept"
    label = "concept"

    object_types = {
        "concept" : ObjType(l_("concept"), "ref")
    }

    directives = {
        "concept" : ConceptDirective
    }

    roles = {
        "ref" : XRefRole()
    }

    initial_data = {
        "objects" : {}
    }

    def clear_doc(self, docname):
        for (typ, name), doc in self.data['objects'].items():
            if doc == docname:
                del self.data['objects'][typ, name]

    def resolve_xref(self, env, fromdocname, builder, typ, target, node,
                     contnode):
        objects = self.data['objects']
        objtypes = self.objtypes_for_role(typ)
        for objtype in objtypes:
            if (objtype, target) in objects:
                return make_refnode(builder, fromdocname,
                                    objects[objtype, target],
                                    objtype + '-' + target,
                                    contnode, target + ' ' + objtype)


def setup(app):
    app.add_crossref_type("concept", "concept", "%s; (concept)")
    #app.add_node(concept)
    #app.add_domain(ConceptDomain)
    #app.add_directive_to_domain("concept", "concept", ConceptDirective)
    #app.add_role_to_domain("ref", "concept", ConceptRef)

