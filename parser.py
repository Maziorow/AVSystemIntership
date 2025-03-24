import json
import subprocess

def sideToEnum(side):
    dictionary = {"north": 0, "south": 1, "east": 2, "west": 3}
    return dictionary[side]

def parse_json_input(input_file):
    with open(input_file, 'r') as f:
        data = json.load(f)
    return data

def generate_c_input(json_data, c_input_file):
    with open(c_input_file, 'w') as f:
        for command in json_data['commands']:
            if command['type'] == 'addVehicle':
                f.write(f"addVehicle {command['vehicleId']} {sideToEnum(command['startRoad'])} {sideToEnum(command['endRoad'])}\n")
            elif command['type'] == 'step':
                f.write("step\n")

def parse_c_output(c_output_file):
    step_statuses = []
    current_step = None

    with open(c_output_file, 'r') as f:
        for line in f:
            line = line.strip()
            if line == "leftVehicles":
                if current_step is not None:
                    step_statuses.append(current_step)
                current_step = {"leftVehicles": []}
            elif line and current_step is not None:
                current_step["leftVehicles"].append(line)

    if current_step is not None:
        step_statuses.append(current_step)

    return {"stepStatuses": step_statuses}

def main(input_file, output_file):
    json_data = parse_json_input(input_file)

    c_input_file = 'c_input.txt'
    generate_c_input(json_data, c_input_file)

    c_output_file = 'c_output.txt'
    subprocess.run(["./main", c_input_file, c_output_file], check=True)

    step_statuses = parse_c_output(c_output_file)

    with open(output_file, 'w') as f:
        json.dump({"stepStatuses": step_statuses}, f, indent=2)

if __name__ == "__main__":
    import sys
    if len(sys.argv) != 3:
        print("Usage: python3 parser.py <input_file> <output_file>")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]
    main(input_file, output_file)