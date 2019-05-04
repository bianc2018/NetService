from locust import HttpLocust, TaskSet, task
import json
import random

from locust.clients import HttpSession
class MyTaskSet(TaskSet):
    host = 'http://127.0.0.1:8000'
    session =None;
    def login(self,json_data):
        login_uri = "/login.action"
        self.session = HttpSession(self.host)
        with self.session.post(url = login_uri,json=json_data,catch_response = True) as response:
            #result = json.loads(response.content)['result']
            if json.loads(response.content)['result'] == 0:
                response.success()
                return response
            else:
                response.failure("login err"+str(json.loads(response.content)['result']))
    def register(self,username,password,admin_password="",permission=3):
        with self.session.post("/register.action",json={"username":username,"password":password,"admin_password":admin_password,"permission":permission},catch_response = True) as response:
             if json.loads(response.content)['result']==0:
                response.success()
                return response
             else:
                response.failure("get_news err"+str(json.loads(response.content)['result']))

    def logout(self,session_id=""):
        login_uri = "/logout.action"
        with self.session.post(url = login_uri,json={"session_id":session_id},catch_response = True) as response:
            #result = json.loads(response.content)['result']
            if json.loads(response.content)['result'] == 0:
                response.success()
                return response
            else:
                response.failure("loginout err "+str(json.loads(response.content)['result']))

    def create_new(self):
        with self.session.post("/create_new.action",catch_response = True) as response:
             if json.loads(response.content)['result']==0:
                response.success()
                return response
             else:
                response.failure("create new err"+str(json.loads(response.content)['result']))

    def get_news(self,where="",size=1000,page=1,byuser=0,orderby="publish_time DESC",transfer_content=0):
        with self.session.post("/get_news.action",json={"where":where,"size":size,"page":page,"byuser":byuser,"orderby":orderby,"transfer_content":transfer_content},catch_response = True) as response:
             if json.loads(response.content)['result']==0:
                response.success()
                return response
             else:
                response.failure("get_news err"+str(json.loads(response.content)['result']))

    def get_new(self,code):
        with self.session.post("/get_new.action",json={"code":code},catch_response = True) as response:
             if json.loads(response.content)['result']==0:
                response.success()
                return response
             else:
                response.failure("get_new err"+str(json.loads(response.content)['result']))
    
    def get_now_user(self):
        with self.session.post("/get_now_user.action",catch_response = True) as response:
             if json.loads(response.content)['result']==0 or json.loads(response.content)['result']==1:
                response.success()
                return response
             else:
                response.failure("get_new err"+str(json.loads(response.content)['result']))

    

    @task
    def on_start(self):
        self.login({'username':"admin","password":"admin"})
        pass

    @task(1)
    def do_register(self):
        r = random.sample('zyxwvutsrqponmlkjihgfedcba',5)
        name = "".join(r);
        per =  random.randint(0,3);
        print name,per
        response = self.register(name,name,"admin",per);
        if json.loads(response.content)['result']==0:
            self.login({'username':name,"password":name});
        pass

    @task(1)
    def do_logout_in(self):
        self.logout()
        self.login({'username':"admin","password":"admin"})

    @task(10)
    def do_index(self):
        self.session.get("/index.html")
        with self.session.get("/index.html",catch_response = True) as response:
             if response.status_code==200:
                response.success()
                self.get_now_user()
                json_news = self.get_news()
                news = json.loads(json_news.content)['news']
                for n in news:
                    if 0 != n['status']:
                        code = n['code'];
                        self.get_new(code);
             else:
                response.failure("create err"+response.content)

  
    @task(2)
    def do_set(self):
        self.session.get("/set.html")
        self.get_now_user()
    
    @task(3)
    def do_new(self):
        self.session.get("/new.html")
        self.get_now_user()

    @task(2)
    def do_login(self):
        self.session.get("/login.html")
        self.get_now_user()
    
    @task(2)
    def do_edit(self):
        self.session.get("/edit.html")
        self.get_now_user()
# r = self.client.get("/homepage/list_header.html")
#        if json.loads((r.content))["result"] != 100:
#            r.failure("Got wrong response:"+r.content)
class MyLocust(HttpLocust):
    task_set = MyTaskSet
    min_wait = 5000
    max_wait = 15000