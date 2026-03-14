# CRUD Exercise

A simple C++ console app performing CRUD operations on a PostgreSQL database, built as a learning exercise.

## Dependencies

### Core

- [CMake](https://cmake.org/) — build system
- [libpqxx](https://pqxx.org/) — PostgreSQL C++ client
- [Docker](https://www.docker.com/) — runs the PostgreSQL instance

### Dev

- [watchexec](https://github.com/watchexec/watchexec) — file watcher for auto-recompilation

## Environment Setup

### Setting up PostgreSQL with Docker

Our CRUD app will be acting on a PostrgreSQL backend. I choose to implement it via a docker container using [postgres:latest](https://hub.docker.com/_/postgres).

Spin up the database container:

```bash
docker run --name crud-postgres \
  -e POSTGRES_PASSWORD=1234 \
  -e POSTGRES_USER=crud \
  -e POSTGRES_DB=classroom \
  -p 5432:5432 \
  -d postgres:latest
```

Start/stop the container:

```bash
docker stop crud-postgres
docker start crud-postgres
```

To open an interactive PostgreSQL session:

```bash
docker exec -it crud-postgres psql -U crud -d classroom
```

### Building the App

### Dev Workflow

To use the [watcher script](scripts/watch.sh) run:

```bash
bash scripts/watch.sh [-n]
```

The option `-n` will prevent the program from running after compilation.

I included the [watcher](scripts/watch.sh) so I can develop the C++ app with more agility. Feel free to inspect it, but it basically follows this structure:

- Spins up a `watchexec` process that calls the [dev script](scripts/dev.sh)
- The dev script calls the [build script](scripts/build.sh) and then spins up the

## Usage
