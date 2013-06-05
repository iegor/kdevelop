
# kdevdir must be set to the KDevelop project directory.
kdevdir=/home/volker/data/prj/kdevelop/kdevelop/
# The plugins are in the parts/ subdirectory.

pushd $kdevdir/parts >/dev/null
oldscope=nix
echo "<para>The following plugin list is generated by a small script"
echo "written by Volker Paul. All plugins have a .desktop file"
echo "where information such as name and comments are written."
echo "If in the following these comments are not very useful,"
echo "it is because the plugin authors made them this way.</para>"
echo "<para>The plugins are grouped by scope (Core, Global, Project).</para>"

for defscope in Core Global Project; do
   if [ $defscope != $oldscope ]; then
      oldscope=$defscope
      echo
#      echo "<sect1><title>Scope: $defscope</title>"
      echo "<para>Scope: $defscope</para>"
      echo "<itemizedlist>"
   fi
   for dir in `ls`; do
	if [ ! -d $dir ]; then continue; fi
	dfile=$dir/*.desktop
	# echo $dfile | wc -w
	if [ `echo $dfile | wc -w` -gt 1 ]; then continue; fi
	if [ ! -f $dfile ]; then continue; fi
	scope=`cat $dfile | grep "X-KDevelop-Scope=" | cut -d= -f2-`
	if [ $scope != $defscope ]; then continue; fi
	comment=`cat $dfile | grep "Comment=" | cut -d= -f2-`
	name=`cat $dfile | egrep "^Name=" | cut -d= -f2- | tr " " _`
	genericname=`cat $dfile | grep "GenericName=" | cut -d= -f2-`
	echo "<listitem><formalpara id=\"$name\">"
	echo "<title>$genericname</title>"
	echo "<para>$comment</para></formalpara></listitem>"
   done
      echo "</itemizedlist>"
#      echo "</sect1>"
done
popd >/dev/null
echo "<para>So far the generated plugin list.</para>"
echo