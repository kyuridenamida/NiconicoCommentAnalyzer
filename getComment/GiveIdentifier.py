#coding:utf-8
# -*- coding:utf-8 -*-
import sqlite3
import sys
import time


class GiveIdentifier:
	def query(self,queryString) : 
		return [_ for _ in self.connector.execute(queryString)];
		
	
	def open(self,dbname): 
		while True:
			try:
				self.connector = sqlite3.connect(dbname)
				self.cur = self.connector.cursor()
				# Word2Number テーブルが存在しない場合作る
				if self.query("select * from sqlite_master where type='table' and name='Word2Number'") == [] : 
					self.query('''create table Word2Number(word,number)''')
				# Number2Word テーブルが存在しない場合作る
				if self.query("select * from sqlite_master where type='table' and name='Number2Word'") == [] : 
					self.query('''create table Number2Word(number,word)''')
				
				#内部辞書等を作る(高速化のため)
				self.innerDic = {}
				self.insertQueue = {}
				for name in ['Number2Word','Word2Number'] : 
					self.innerDic[name] = {}
					self.insertQueue[name] = []
					for x in self.query("select * from %s" % name):
						self.innerDic[name][x[0]] = x[1];
				# if len(self.innerDic['Number2Word']) != len(self.innerDic['Word2Number']) :
				# print len(self.innerDic['Number2Word']) , len(self.innerDic['Word2Number'])
					#sys.exit(1)
				break
				
			except :
				time.sleep(1)
		

	
	def word2number(self,word):
		word = word.decode('utf-8')
		if word in self.innerDic['Word2Number'] :
			return self.innerDic['Word2Number'][word];
		else:
			numberOfRecords = len(self.innerDic['Word2Number'])
			self.innerDic['Word2Number'][word] = numberOfRecords
			self.innerDic['Number2Word'][numberOfRecords] = word
			self.insertQueue['Word2Number'].append((word,numberOfRecords));
			self.insertQueue['Number2Word'].append((numberOfRecords,word));
			return numberOfRecords
	# def word2number(self,word) : 
		# res = self.query("select number from Word2Number where word='%s'" % word);
		# if res != [] :
			# return res[0][0]
		# else : 
			# numberOfRecords = self.query("select count(*) from Word2Number")[0][0]
			# self.query("insert into Word2Number values('%s',%d)" % (word,numberOfRecords) )
			# self.query("insert into Number2Word values(%d,'%s')" % (numberOfRecords,word) )
			# return numberOfRecords
	
	def wordExists(self,word) : 
		word = word.decode('utf-8')
		return word in self.innerDic['Word2Number'];
	# def wordExists(self,word) : 
		# res = self.query("select number from Word2Number where word='%s'" % word);
		# return res != []
	
	def wordExistsOneTime(self,dbname,word) : 
		while True:
			try:
				self.connector = sqlite3.connect(dbname)
				self.cur = self.connector.cursor()
				res = self.query("select number from Word2Number where word='%s'" % word);
				self.connector.close()
				return res != []
			except :
				time.sleep(1)
	
	def number2word(self,number) :
		return self.innerDic['Number2Word'][number];
	# def number2word(self,number) :
		# res = self.query("select word from Number2Word where number=%d" % number);
		# return res[0][0]
	
	def close(self) : 
		for name in ['Number2Word','Word2Number'] : 
			groupSize = 1000
			for subList in [self.insertQueue[name][i:i+groupSize] for i in range(0,len(self.insertQueue[name]),groupSize)]: 
				queryStr = u"insert into %s values" % name;
				for ele in subList : 
					queryStr += u"(%s,%s)," % (
						(str(ele[0]) if isinstance(ele[0],int) else u"'"+ele[0]+u"'"),
						(str(ele[1]) if isinstance(ele[1],int) else u"'"+ele[1]+u"'")
					);
				self.query(queryStr[:-1])
			
		self.connector.commit()
		self.connector.close()
	# def close(self) : 
		# self.connector.commit()
		# self.connector.close()
if __name__ == '__main__':
	dic = GiveIdentifier()
	dic.open("sqlite_test.db")
	
	dic.close()
	
