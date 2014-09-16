export TARGET=arm-none-eabi
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
export PREFIX="$DIR/binutils-pi"
function grab () {
    echo "Pulling $1..."
    if [ ! -f "$3" ]; then
        wget "$2/$3"
    else
        echo "Already have $1"
    fi
} #Credit Kevin Lange (klange) for grab function
echo "Building binutils"
grab "binutils" "http://ftp.gnu.org/gnu/binutils" "binutils-2.22.tar.gz"
echo "Extracting..."
tar -xf binutils-2.22.tar.gz
mkdir binutils-pi
mkdir build-binutils-pi
cd build-binutils-pi
echo "Installing to $PREFIX"
../binutils-2.22/configure --target=$TARGET --prefix=$PREFIX --disable-nls
make all
make install
