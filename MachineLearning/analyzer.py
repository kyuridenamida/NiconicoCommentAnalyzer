# -*- coding: utf-8 -*-
import GiveIdentifier
import sys;

dbname = 'sqlite_test.db';
dictionary = GiveIdentifier.GiveIdentifier();
dictionary.open(dbname)
for line in sys.stdin :
	arr = line.strip().split(" ");
	print dictionary.number2word(int(arr[0])), arr[1]
dictionary.close()