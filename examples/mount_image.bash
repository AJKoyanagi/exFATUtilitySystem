#!/bin/bash
# this script will mount an image
# it takes 2 arguments, first being the name of the image and the second being
# the mount point.
#
if [ $# -eq 2 ]
then
   # Check for the mount point
   if [ -d $2 ]
   then
      echo "Mount Point Exists."
   else 
      echo "Mount Point Does not Exist"
      echo "creating $2"
      mkdir -p $2
   fi
   echo "there are 2 of them"
else
   echo "This script requires 2 arguments."
   echo "For example:"
   echo "bash example/mount_image.bash test.image /tmp/d"
fi