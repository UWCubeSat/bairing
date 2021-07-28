The Arduino IDE enforces a directory structure where all non-library files must either be in the same folder as the sketch, or under /src/.

/src/: Blueboy-specific configuration and logic, encapsulated to keep away from the main sketch
  -> /sensor/: Sensor-related libraries or utilities, such as drivers or calibrators.
  -> /util/: General-purpose libraries or utilities, like packet processors.