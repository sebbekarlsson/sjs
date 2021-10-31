import os
import subprocess
import datetime
import sys
import requests

from jinja2 import Template, Environment, FileSystemLoader

from functools import reduce

import tarfile

import urllib

NODE_VERSION = "16.13.0"

DIRNAME = os.path.dirname(__file__)
SJS_BIN = os.path.join(DIRNAME, '..', 'build', 'sjs')
NODE_BIN = os.path.join(DIRNAME, 'node', f'node-v{NODE_VERSION}-linux-x64', 'bin', 'node')

IS_PYTHON_3 = sys.version_info >= (3, 0)

OUTNAME = os.path.join(DIRNAME, '../', 'benchmarks.md')

JINJA_ENV = Environment(loader=FileSystemLoader(
                os.path.abspath(
                    os.path.dirname(__file__)) + '/templates', encoding='utf8'))


def untar(fname, outdir):
    p = subprocess.Popen(['tar', 'xf', fname, '-C', outdir])
    return p.communicate()


def download_file(url, filename):
    r = requests.get(url, allow_redirects=True)
    open(filename, 'wb').write(r.content)


def download_node(version):
    filename = "node-v{}-linux-x64.tar.xz".format(version)
    filename_on_disk = os.path.join(DIRNAME, filename)
    NODE_DIR = os.path.join(DIRNAME, "node")

    if os.path.isfile(filename_on_disk):
        if not os.path.isdir(NODE_DIR):
            os.mkdir(NODE_DIR)
            untar(filename_on_disk, NODE_DIR)
        return

    URL = "https://nodejs.org/dist/v{}/{}".format(version, filename)
    download_file(URL, filename_on_disk)
    untar(filename_on_disk, NODE_DIR)


def sjs_run(filename):
    fullpath = os.path.join(DIRNAME, 'sourcefiles', filename)
    cmd = [SJS_BIN, fullpath]
    p = subprocess.Popen(cmd)
    return p.communicate(), cmd


def node_run(filename):
    fullpath = os.path.join(DIRNAME, 'sourcefiles', filename)
    cmd = [NODE_BIN, fullpath]
    p = subprocess.Popen(cmd)
    return p.communicate(), cmd

def run_js(filename, func):
    date1 = datetime.datetime.now()
    p, cmd = func(filename)
    date2 = datetime.datetime.now()

    first = cmd[0]
    last = cmd[-1]
    bin_name = os.path.basename(first)
    cmd = [bin_name, *cmd[1:]]
    joined_cmd = ' '.join(cmd)

    diff = date2 - date1

    code = open(last, 'r+').read()

    return dict(cmd=joined_cmd, time=diff, filename=filename, date=datetime.datetime.now(), code=code)


import glob

ALL_FILES = list(map(lambda x: os.path.basename(x), glob.glob(os.path.join(DIRNAME, 'sourcefiles', '*.js'))))

MARKS = reduce(lambda a, b: {**a, b: [sjs_run, node_run]}, ALL_FILES, dict())

def main():
    download_node(NODE_VERSION)


    files = list(MARKS.keys())

    result = reduce(lambda prev, fname: {**prev, fname: dict(runs=(prev[fname] if fname in prev else []) + list(map(lambda func: run_js(fname, func), MARKS[fname])))}, files, dict())
    marks = list(map(lambda filename: dict(filename=filename, **result[filename], code=result[filename]['runs'][0]['code']), files))

    interpreter_content = JINJA_ENV.get_template("interpreter.md").render(marks=marks)
    main_content = JINJA_ENV.get_template("main.md").render(now=datetime.datetime.now(), contents=[interpreter_content])

    open(OUTNAME, 'w+').write(main_content)



main()
