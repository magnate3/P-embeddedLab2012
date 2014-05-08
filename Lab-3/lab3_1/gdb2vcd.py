import re

def main():
	qTaskName=[];
	qTickCount=[];

	fptr = open("gdb.log", "r");
	for line in fptr:
		line = line.rstrip();
#		print "["+line+"]";
		found = re.search('pxCurrentTCB->pcTaskName = \"(([^\"](?!000))*)', line);
		if(found):
			print "pcTaskName = "+found.group(1);
			qTaskName.append(found.group(1));
		found = re.search('xTickCount = (.*)', line);
		if(found):
#			print "xTickCount = "+found.group(1);
			qTickCount.append(found.group(1));
	fptr.close();
	#
	print qTaskName;
	print qTickCount;

if __name__ == "__main__":
    main()
