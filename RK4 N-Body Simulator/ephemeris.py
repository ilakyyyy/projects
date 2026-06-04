import urllib.request
import urllib.parse
import sys

bodies = {
    "Sun": "10",
    "Venus": "299",
    "Earth": "399",
    "Moon": "301",
    "Jupiter": "599",
    "Solar Orbiter": "-144"
}

url = "https://ssd.jpl.nasa.gov/api/horizons.api"

with open("ephemeris.txt", "w") as f:
    for name, obj_id in bodies.items():
        print(f"  -> Downloading {name}...")
        params = {
            "format": "text",
            "COMMAND": f"'{obj_id}'",
            "MAKE_EPHEM": "YES",
            "EPHEM_TYPE": "VECTORS",
            "CENTER": "'@0'", #Solar system barycenter
            "START_TIME": "'2020-02-11 00:00:00'",
            "STOP_TIME": "'2020-02-12 00:00:00'",
            "STEP_SIZE": "'1d'",
            "OUT_UNITS": "'KM-S'",
            "CSV_FORMAT": "'YES'"
        }
        query = urllib.parse.urlencode(params)
        
        with urllib.request.urlopen(f"{url}?{query}") as response:
            text = response.read().decode('utf-8')
            
        #Extract the CSV line between $$SOE and $$EOE
        start = text.find("$$SOE")
        if start == -1:
            print(f"\n[ERROR] No ephemeris data found for {name} at the requested time.")
            print(f"API Response snippet: {text[:250]}")
            sys.exit(1)
            
        end = text.find("$$EOE")
        data_line = text[start+5:end].strip().split('\n')[0]
        parts = data_line.split(',')
        
        #Convert km and km/s to m and m/s
        x, y, z = float(parts[2])*1000, float(parts[3])*1000, float(parts[4])*1000
        vx, vy, vz = float(parts[5])*1000, float(parts[6])*1000, float(parts[7])*1000
        
        f.write(f"{x} {y} {z} {vx} {vy} {vz}\n")

print("\n[SUCCESS] ephemeris.txt generated! The Solar System is ready.")