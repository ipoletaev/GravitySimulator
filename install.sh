#!/usr/bin/env bash

DEST_DIR="venv";         # folder where the virtual environment will be installed

command -v python -m pip >/dev/null 2>&1 || { # Check if pip is installed
    echo "No 'pip' is installed. Aborting..." >&2;
    exit 1;
}


command -v python -m virtualenv >/dev/null 2>&1 || { # Check if virtualenv is installed
    echo "No 'virtualenv' is installed. Installing ..." >&1;
    python -m pip install virtualenv || { # Try to install a virtualenv if none is found
        echo "Installation of 'virtualenv' failed. Aborting..." >&2;
        exit 1;
    }
}


if [ -d $DEST_DIR ]; then
    echo "You are about to delete '$DEST_DIR'. Do you want to proceed (Y/N)?";
    read ANSWER;
    ANSWER=$(echo "$ANSWER" | awk '{print toupper($0)}');
    case "$ANSWER" in
        Y | YES)
            echo "Deleting '$DEST_DIR'...";
            rm -Rf $DEST_DIR;
        ;;
        *)
            echo "Unable to create a virtual environment in '$DEST_DIR'. Aborting..." >&2;
            exit 1;
        ;;
    esac
fi


echo "Creating a virtual environment in '$DEST_DIR'"
python -m virtualenv "$DEST_DIR" || {
    echo "Failed to create a virtual environment in '$DEST_DIR'. Aborting..." >&2;
    exit 1;
}

pushd "$DEST_DIR" >/dev/null
    DEST_DIR=$(pwd)
popd >/dev/null

source $DEST_DIR"/bin/activate" || {
    echo "Failed to activate the created virtual environment" >&2;
    exit 1;
}
export PYTHONPATH="${PYTHONPATH}:$DEST_DIR/local/lib/python2.7/dist-packages/:$DEST_DIR/local/lib64/python2.7/dist-packages/"


pip install pip==9.0.1 || {
    echo "Failed to upgrade 'pip' to v.9.0.1" >&2;
    exit 1;
}


pip install --upgrade -r ./requirements.txt || {
    echo "Failed to install / upgrade one or more items listed in requirements.txt" >&2;
    exit 1;
}


pushd ./src || exit 1
    echo "Compile sources for generator test..."
    make || {
        echo "Failed to compile sources..." >&2;
        exit 1;
    }
    python setup.py install || {
        echo "'python setup.py install' failed in './src'" >&2;    
        exit 1;
    }
popd


#deactivate # command to deactivate virtual environment


echo "Everything for usage is ready!"
