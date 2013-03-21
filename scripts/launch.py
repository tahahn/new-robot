#!/usr/bin/env python

import json
import os, sys, errno
import signal, threading
import shlex, subprocess

from argparse import *

def findFile(filename, *args):
    #If the filename is already a valid path, use that.
    if os.path.isfile(filename):
        return filename

    #Otherwise, iterate over all the specified directories,
    # as well as the os path directories.
    paths = list(args) + os.environ['PATH'].split(':')
    for path in paths:
        val = os.path.join(path, filename)
        if os.path.isfile(val):
            return val

    return None

processes = []

def main():
    #Parse the arguments from the command line.
    parser = ArgumentParser()
    parser.add_argument("-c", "--config", required = True,
        help = "Specify the configuration file for this program to use.")
    args = parser.parse_args()

    #Set some useful variables for later.
    #TODO: Un-hardcode the top_path somehow, so we can use this program in other folders.
    top_dir = os.path.abspath(os.path.join(sys.argv[0], "../.."))
    config_dir = os.path.join(top_dir, "config")
    build_dir = os.path.join(top_dir, "build")

    #Get the filepath of the specified config file.
    config_file = findFile(args.config, config_dir, top_dir)

    if config_file == None:
        print 'Could not find the specified configuration file, aborting...'
        return

    #Open the specified configuration file.
    try:
        f = open(config_file, 'r')
        config = json.loads(f.read())
        f.close()

        #Add the "subprocess_data" section to the configuration, so we can send it to the subprocesses.
        config["subprocess_data"] = {"top_dir" : top_dir}

        print 'Loaded configuration file ' + config_file + '.'

    except IOError:
        print 'Could not open the configuration file at ' + config_file + ', aborting...'
        return

    #Go through the config data and start all of the subprocesses.
    try:
        #Iterate through each process specified in the config file.
        for k, v in config['subprocesses'].iteritems():

            #Get the command for the process from the config file.
            if "command" in v:
                command = shlex.split(v["command"])
            else:
                command = [k]

            #Find the program in the filesystem.
            if "directory" in v:
                command[0] = findFile(command[0], v["directory"], build_dir, top_dir)
            else:
                command[0] = findFile(command[0], build_dir, top_dir)

            #If the program could not be found, abort.
            if command[0] == None:
                print 'Could not find subprocess ' + k + ', aborting...'
                raise KeyboardInterrupt

            #Put the subprocess data in the configuration.
            config["subprocess_data"]["name"] = k
            config["subprocess_data"]["prog_dir"] = command[0]

            print 'Starting subprocess ' + k + ' with ' + command[0] + '...'

            #Start the process, and write the configuration to stdin.
            p = subprocess.Popen(command, stdin = subprocess.PIPE, stdout = subprocess.PIPE, stderr = subprocess.PIPE)
            p.stdin.write(json.dumps(config, separators = (',',':')) + '\n')
            p.stdin.close()
            processes.append(p)

            print 'Subprocess ' + k + ' started.'

        print 'All subprocesses started.'

        for p in processes:
            p.wait()

        print 'All subprocesses finished.'
        return

    except OSError:
        print 'Error while starting subprocess: ' + os.strerror(errno.errorcode) + ', aborting...'

    except KeyboardInterrupt:
        print '\nInterrupting all subprocesses...'
        print 'Press Ctrl-C again to force termination.'
        for p in processes:
            p.send_signal(signal.SIGINT)

    #If the program was stopped short, try to recover by ending all the subprocesses.
    try:
        for p in processes:
            p.wait()

        print 'All subprocesses finished.'

    except KeyboardInterrupt:
        for p in processes:
            p.send_signal(signal.SIGTERM)

        print '\nAll subprocesses terminated.'

if __name__ == "__main__":
    main()
