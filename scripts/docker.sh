if docker ps -a --format '{{.Names}}' | grep -q '^crud-postgres$'; then
  echo "Container already exists, starting it..."
  docker start crud-postgres
else
  echo "Creating container..."
  docker run --name crud-postgres \
    -e POSTGRES_PASSWORD=1234 \
    -e POSTGRES_USER=crud \
    -e POSTGRES_DB=classroom \
    -p 5432:5432 \
    -d postgres:latest
fi

docker start crud-postgres

docker exec -it crud-postgres psql -U crud -d classroom
