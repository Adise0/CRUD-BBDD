#!/usr/bin/env bash
set -euo pipefail

clear

if docker ps -a --format '{{.Names}}' | grep -q '^crud-postgres$'; then
  echo "Container already exists, starting it..."
else
  echo "Creating container..."
  docker run --name crud-postgres \
    -e POSTGRES_PASSWORD=1234 \
    -e POSTGRES_USER=crud \
    -e POSTGRES_DB=classroom \
    -p 5432:5432 \
    -d postgres:latest

  echo "Waiting for PostgreSQL to be ready..."
  until docker exec crud-postgres pg_isready -U crud -d classroom > /dev/null 2>&1; do
    sleep 1
  done

  echo "Setting up schema..."
  docker exec -i crud-postgres psql -U crud -d classroom < sql/2-CreatingStructures_Table-Enum.sql
fi

docker start crud-postgres
docker exec -it crud-postgres psql -U crud -d classroom
