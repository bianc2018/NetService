//获取当前会话的用户

function user_status()
{
    session_id = get_session_id();

    if (session_id == null) {
       // alert("获取用户数据失败 ret=" + data.result);
        $('#person').append('<a href="login.html">游客登录/注册</a>');
        return -1;
    }
    else {
       
        $.post('/get_now_user.action', '{"session_id":"' + session_id + '"}', function (data) {
            if (data.result < 0) {
                alert("获取用户数据失败 " + Error_Code(data.result));
                window.location.href = "/index.html";
                return -2;
            }
            else if (data.result == 0) {
                if (data.user.username == null || data.user.username == "")
                {
                    rm_session('');
                    return;
                }
                //$('#person').append('<a href="set.html">' + data.user.username + '(' + session_id + ') 的个人空间</a>&nbsp;<a href="/logout.action">登出</a>');
                $('#person').append('<a href="set.html">' + data.user.username + ' 的个人空间</a>&nbsp;<a style="cursor: pointer;" onclick="rm_session(\'\')">登出</a>');
                //$('#forward').append('<a href="edit.html">新闻发布</a>  ');
                $('#forward').append('<a href="set.html">设置页</a>');
                //new.html
                //权限 0 管理员 1 编辑 2 用户 3 游客
                if(data.user.permission>0)
                {
                    //非管理员不可对用户进行操作
                    $('#users_list').hide();
                    $('#sessions_list').hide();
                    if (data.user.permission > 1)
                    {
                        //非编辑者不可对新闻进行操作
                        $('#news_list').hide();
                        
                    }
                }

            }
            else {
                $('#person').append('<a href="login.html">游客登录/注册</a>');
                return 1;
            }
        }, 'json'
        );
    }
}
function rm_session(id) {
    if (id == "")
        id = get_session_id();
    
    $.post("/logout.action", '{"session_id":"' + id + '"}', function (data) {
        if (data.result == 0) {
            //window.location.href = '/set.html?menu=news_list';
            if (get_session_id() == id) {
                alert("当前会话已结束！！！\n请重新登录");
                window.location.href = '/login.html';
            }
            else
                load_sessions_list();
        }
        else {
            if (data.result != null) {
                alert("删除失败：" + Error_Code(data.result));
            }
        }
    })
}