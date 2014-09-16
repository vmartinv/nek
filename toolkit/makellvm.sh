echo "Building llvm/clang..."
echo "Requires SVM, cmake, make, and binutils"
mkdir cross
cd cross
svn co http://llvm.org/svn/llvm-project/llvm/trunk llvm
cd ./llvm/tools
svn co http://llvm.org/svn/llvm-project/cfe/trunk clang
cd ../..
mkdir build
cd build
cmake ../llvm
make
