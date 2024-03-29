# Description

This project represents a simulation, where _sensors_ send data to a _controller_, which makes a decision based on the data and sends it to the _manipulator_. The _controller_ also provides a HTTPS API, so that a 3rd-party servers can access the decisions made by the _controller_.

> **Important!**
>
> The project makes use of submodules.
>
> Pull them manually after clonning the repository, by running the following commands:
>
> ```bash
> git submodule init
> git submodule update
> ```
> 
> Or add `--recurse-submodules` to the `git clone` (this option will clone all submodules recursively, so the size of the resulting cloned repository might be big).
>

# Running the app

Dependencies:

- POCO
- OpenSSL and/or libssl-dev

If one runs the system in the `docker`, there is no need to bother with the dependencies.

## Docker

### Docker-compose

Run the simulation using `docker-compose`:

```bash
cd docker
docker-compose up --scale sensor=8
```

This command will run the simulation via `docker-compose` with **8** sensors.

One will be able to observe the console output of both a `manipulator` and a `controller` in the logs. In addition a flask application is available at `127.0.0.1:5000` to send requests to the controller to retrieve its decisions.

To build all necessary images, one can first run `dcoker-compose build` or specify `--build` option in addition to `dcoker-compose up`.

### Building an image

Example of building a `docker` image:

```bash
cd docker
docker build -t sensor:test -f builtCppPoco.Dockerfile --build-arg APP_NAME=sensor ..
```

This script will build an image with the `sensor` app.

When building an image for the first time it might take some time, as it needs to build a `Poco` library first. Once the stage of building `Poco` has passed, `docker` will use its cache to build images with other apps faster.
