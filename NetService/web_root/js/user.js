//��ȡ��ǰ�Ự���û�

function user_status()
{
    session_id = get_session_id();

    if (session_id == null) {
       // alert("��ȡ�û�����ʧ�� ret=" + data.result);
        $('#person').append('<a href="login.html">�ο͵�¼/ע��</a>');
        return -1;
    }
    else {
       
        $.post('/get_now_user.action', '{"session_id":"' + session_id + '"}', function (data) {
            if (data.result < 0) {
                alert("��ȡ�û�����ʧ�� " + Error_Code(data.result));
                window.location.href = "/index.html";
                return -2;
            }
            else if (data.result == 0) {
                if (data.user.username == null || data.user.username == "")
                {
                    rm_session('');
                    return;
                }
                //$('#person').append('<a href="set.html">' + data.user.username + '(' + session_id + ') �ĸ��˿ռ�</a>&nbsp;<a href="/logout.action">�ǳ�</a>');
                $('#person').append('<a href="set.html">' + data.user.username + ' �ĸ��˿ռ�</a>&nbsp;<a style="cursor: pointer;" onclick="rm_session(\'\')">�ǳ�</a>');
                //$('#forward').append('<a href="edit.html">���ŷ���</a>  ');
                $('#forward').append('<a href="set.html">����ҳ</a>');
                //new.html
                //Ȩ�� 0 ����Ա 1 �༭ 2 �û� 3 �ο�
                if(data.user.permission>0)
                {
                    //�ǹ���Ա���ɶ��û����в���
                    $('#users_list').hide();
                    $('#sessions_list').hide();
                    if (data.user.permission > 1)
                    {
                        //�Ǳ༭�߲��ɶ����Ž��в���
                        $('#news_list').hide();
                        
                    }
                }

            }
            else {
                $('#person').append('<a href="login.html">�ο͵�¼/ע��</a>');
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
                alert("��ǰ�Ự�ѽ���������\n�����µ�¼");
                window.location.href = '/login.html';
            }
            else
                load_sessions_list();
        }
        else {
            if (data.result != null) {
                alert("ɾ��ʧ�ܣ�" + Error_Code(data.result));
            }
        }
    })
}