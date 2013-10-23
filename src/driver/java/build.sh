#!/bin/bash

# define compile
JAVA_HOME=../../../java/jdk_linux64
JAVAC=$JAVA_HOME/bin/javac
JAR=$JAVA_HOME/bin/jar

#define source directory
SOURCE_DIR=./src

#define bin directory
BIN_DIR=./bin

# define target directory
TARGET_DIR=../../../bin

if [ ! -d $BIN_DIR ]; then
   mkdir $BIN_DIR
fi

if [ ! -d $TARGET_DIR ]; then
   mkdir $TARGET_DIR
fi

# delete all .class file
find . -name "*.class" |xargs rm -rf

$JAVAC `find $SOURCE_DIR -name "*.java" -print` -d $BIN_DIR

cd $BIN_DIR

l=`find ./ -name "*.class" -print | sed 's/^..//'`
../$JAR -cf ../$TARGET_DIR/emeralddb.jar $l
cd ..

# clean
find ./ -name "*.class" | xargs rm -rf
rm -rf $BIN_DIR
