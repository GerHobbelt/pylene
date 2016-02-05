doc:
	sphinx-build -b html source build

doxy:
	doxygen
	#/lrde/home/stage/carlinet/Downloads/doxygen/bin/doxygen

extract:
	#sudo systemctl start mongod.service
	python extract-doxygen.py doxyxml pylene


create:
	python create-rst.py pylene

clean :
	rm -Rf build
