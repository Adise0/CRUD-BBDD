-- Creating the types and table for the computers

CREATE TYPE computer_type AS ENUM (
  'WORKSTATION',
  'GAMING',
  'LAPTOP'
);


CREATE TYPE computer AS (
  name        VARCHAR(100),
  n_of_users  INTEGER,
  is_active   BOOLEAN,
  type        computer_type
);

CREATE TABLE computers (
  id       SERIAL PRIMARY KEY,
  computer computer
);


-- Inserting 3 records into the computers table
INSERT INTO computers (computer) VALUES
(ROW('CLASS 0.4 PC 1', 10, true, 'WORKSTATION')),
(ROW('Gaming Rig', 5, true, 'GAMING')),
(ROW('Adise Laptop', 1, false, 'LAPTOP'));
