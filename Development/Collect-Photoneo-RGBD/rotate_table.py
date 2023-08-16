import sys
import time
from ctypes import c_int, byref
import libximc as ximc

def rotate_table(degrees_to_rotate, return_to_zero=False):
    # Inser correct device URI if different

    device_uri = "xi-com:///dev/ximc/000042A6"  # Serial port

    device_id = ximc.lib.open_device(device_uri.encode())
    if device_id > 0:
        print("Device with URI {} successfully opened".format(device_uri))
    else:
        raise RuntimeError("Failed to open device with URI", device_uri)

    x_pos = ximc.get_position_t()
    result = ximc.lib.get_position(device_id, byref(x_pos))
    if result == ximc.Result.Ok:
        print("Position: {0} steps, {1} microsteps".format(x_pos.Position, x_pos.uPosition))

    print("Launch movement...")

    # Calculate the number of steps for the given degrees
    steps_per_degree = 67
    position_in_steps = int(steps_per_degree * degrees_to_rotate)
    
    # Calculate the target position
    target_position = x_pos.Position + position_in_steps

    print("Target position in steps: " + str(target_position))

    # Move to the target position
    ximc.lib.command_move(device_id, target_position)
    
    # Wait for the goal to be reached
    while True:
        result = ximc.lib.get_position(device_id, byref(x_pos))
        if result == ximc.Result.Ok:
            current_position = x_pos.Position
            print("Current position in steps: " + str(current_position))
            if current_position + 1 >= target_position:
                break
        time.sleep(0.2)

    print("Goal reached at position: {0} steps, {1} microsteps".format(x_pos.Position, x_pos.uPosition))

    if return_to_zero:
        print("Returning to position 0...")
        ximc.lib.command_move(device_id, 0)
        # Wait for the return to zero
        while True:
            result = ximc.lib.get_position(device_id, byref(x_pos))
            if result == ximc.Result.Ok:
                current_position = x_pos.Position
                if current_position == 0:
                    break
            time.sleep(0.2)

        print("Returned to position 0")

    print("Disconnect device")
    ximc.lib.close_device(byref(c_int(device_id)))

    print("Done")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python script.py <degrees_to_rotate> [--return-to-zero]")
        sys.exit(1)

    degrees_to_rotate = float(sys.argv[1])
    return_to_zero = False

    if "--return-to-zero" in sys.argv:
        return_to_zero = True

    rotate_table(degrees_to_rotate, return_to_zero)
