#coding:utf-8
# -*- coding: utf-8 -*-

import json
import urllib2, cookielib
from urlparse import parse_qs
import re

class NicoInfoGetter:
	def __init__(self,username,password):
		cj = cookielib.CookieJar()              # Cookieを格納するオブジェクト
		cjhdr = urllib2.HTTPCookieProcessor(cj) # Cookie管理を行うオブジェクト
		self.opener = urllib2.build_opener(cjhdr)    # OpenDirectorオブジェクトを返す
		self.opener.open("https://secure.nicovideo.jp/secure/login","mail=%s&password=%s" % (username,password) );
	
	# 動画のタグ情報とコメント情報を返します．
	def getComment(self,smID):
		# smIDからthreadIDの取得
		r = self.opener.open('http://flapi.nicovideo.jp/api/getflv/' + smID)
		html = r.read()
		res = parse_qs(urllib2.unquote(html))		
		
		# threadIDからコメントの取得
		r = self.opener.open("%sthread?version=20090904&thread=%s&res_from=-1000" % (res['ms'][0],res['thread_id'][0]) )
		xml = r.read()
		
		# コメントの取得
		rm = re.compile(">(.*)</chat")
		comments = []
		for line in xml.split('><') :
			g = rm.search(line);
			if( g != None ) : comments.append(unicode(g.group(1),'utf-8').replace(u'　','').replace(u' ','').replace(u'\'','').replace(u'\"',''))
		return comments 
	
	def getInformation(self,smID):
		r = self.opener.open('http://ext.nicovideo.jp/api/getthumbinfo/' + smID)
		xml = r.read()
		
		res = {}
		#タイトルを取得
		res['title'] = re.compile("<title>(.*)</title>").search(xml).group(1);
		#タグを取得
		tags = []
		rm = re.compile(">(.*)</tag")
		for line in xml.split('\n') :
			g = rm.search(line);
			if( g != None ) : tags.append(g.group(1))
		
		res['tags'] = tags;
		#コメントを取得(別メソッド)
		res['comments'] = self.getComment(smID);
		return res;
	
	def tagSearch(self,keyword,howmany) : 
		
		lst = []
		for i in xrange(0,howmany,100):
			data = {
			  "query" : keyword,
			  "service" : ["video"],
			  "search" :["tags_exact"],
			  "join" : ["cmsid","title"],
			  "filters" : [{"type":"range","field":"view_counter","to":100000}], # 10万再生以下
			  "sort_by" :  "comment_counter",
			  "order" : "desc",
			  "from" : i,
			  "size" : min([howmany-i,100]),
			  "issuer" : "NicoInfoGetter"
			};
			self.opener.addheaders = [('Content-Type', 'application/json')];
			r = self.opener.open('http://api.search.nicovideo.jp/api/snapshot/',json.dumps(data))
			jsondata = r.read().split('\n')[0]
			lst += [x['cmsid'] for x in json.loads(jsondata)['values']]
		
		return lst
			
		
if __name__ == '__main__':
	infoGetter = NicoInfoGetter("*****.email", "******");
	print infoGetter.tagSearch(u"真夏の夜の淫夢",1000)
	