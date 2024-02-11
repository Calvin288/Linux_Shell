# Linux_Shell

Compile the myshell.c file and run the executable to start the shell.

Once the shell starts, try different linux commands such as:
1. ```ls```
2.  ```cd```
3.  ```pwd```

Afterward, you can try more complex redirection commands such as:
1. ```wc -l < myshell.c```
2. ```ls -lh > tmp_out_only.txt```
3. ```wc -l < myshell.c > tmp_in_then_out.txt```
4. ```wc -l > tmp_out_then_in.txt < myshell.c```

Lastly, the shell implements multi-level pipes to connect output from the first command to the second command, like:
1. ```ls | sort```
2. ```ls | sort -r | sort | sort -r | sort | sort -r | sort | sort -r```

To end shell, enter ```exit```
