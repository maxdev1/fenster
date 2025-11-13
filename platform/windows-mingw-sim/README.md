# Docker file

The Docker image automatically builds all required libraries for building the Windows binary.
To build the image:

	docker build -t mingw-freetype-cairo .
	docker run -it --rm -v C:/Code/ghost/applications/fenster:/build mingw-cairo-freetype

Login:

    echo <github-token> | docker login ghcr.io -u <user> --password-stdin
	docker tag mingw-cairo-freetype ghcr.io/<user>/mingw-cairo-freetype:latest
	docker push ghcr.io/<user>/mingw-cairo-freetype:latest
