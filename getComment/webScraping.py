# -*- coding: utf-8 -*-
import NicoInfoGetter
import GiveIdentifier
import sys;
import time;
from janome.tokenizer import Tokenizer

dbname = 'sqlite_test.db';
t = Tokenizer()
dictionary = GiveIdentifier.GiveIdentifier();
def unicode2Number(word) :
	return dictionary.word2number(word)
	
infoGetter = NicoInfoGetter.NicoInfoGetter("****@****.email", "114514");

traindata = open("trainingdata.txt","a")
for kw in [u'泣ける淫夢',u'涙腺崩壊']:
	movies = infoGetter.tagSearch(kw,1000);
	for smid in movies: 
		if( dictionary.wordExistsOneTime(dbname,smid) ):
			print >> sys.stderr, smid , "is already exists."
			continue
		
		
		for i in range(1,100):
			try:
				res = infoGetter.getInformation(smid);
				break
			except:
				print >> sys.stderr, smid , "network error"
				time.sleep(1)
		
		print >> sys.stderr, "Processing",smid,"...";
		#辞書を読み込む
		dictionary.open(dbname)
		print >> sys.stderr, "dictinary read done",smid,"...";
		wordList = []
		
		#形態素解析
		for i in res['comments']:
			try:
				for token in t.tokenize(i):
					wordList.append(token.surface.encode('utf-8'))
			except :
				''''''
		
		# print >> sys.stderr, "dictinary read done",smid,"...";
		#タイトル文字列をidにもする
		print >> traindata, unicode2Number(smid), smid
		#タグの数を出力
		print >> traindata, len(res['tags'])
		#タグの内容を出力
		for x in [unicode2Number(x) for x in res['tags']]:
			print >> traindata, x,
		print >> traindata
		#コメント内容を出力
		print  >> traindata,len(wordList)
		for x in [unicode2Number(x) for x in wordList]:
			print >> traindata,x,
		print >> traindata
		traindata.flush()
		dictionary.close()
		print "--",smid,"-- *done*"
traindata.close()