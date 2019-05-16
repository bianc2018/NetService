function get_query_string(name) {
    var url = window.location.search.substr(1);

    var params = url.split('&');
    for (var i = 0; i < params.length; i++) {
        var pair = params[i].split('=');
        if (name == pair[0])
            return pair[1];
    }

    return null;
}
//дcookies 

function setCookie(name, value) {
    var Days = 30;
    var exp = new Date();
    exp.setTime(exp.getTime() + Days * 24 * 60 * 60 * 1000);
    document.cookie = name + "=" + escape(value) + ";expires=" + exp.toGMTString();
}

//��ȡcookies 
function getCookie(name) {
    var arr, reg = new RegExp("(^| )" + name + "=([^;]*)(;|$)");

    if (arr = document.cookie.match(reg))

        return unescape(arr[2]);
    else
        return null;
}

//ɾ��cookies 
function delCookie(name) {
    var exp = new Date();
    exp.setTime(exp.getTime() - 1);
    var cval = getCookie(name);
    if (cval != null)
        document.cookie = name + "=" + cval + ";expires=" + exp.toGMTString();
}
//ʹ��ʾ�� 
//setCookie("name", "hayden");
//alert(getCookie("name"));

//�����Ҫ�趨�Զ������ʱ�� 
//��ô�������setCookie������������������������ok; 


//������� 
function setCookie(name, value, time) {
    var strsec = getsec(time);
    var exp = new Date();
    exp.setTime(exp.getTime() + strsec * 1);
    document.cookie = name + "=" + escape(value) + ";expires=" + exp.toGMTString();
}
function getsec(str) {
    alert(str);
    var str1 = str.substring(1, str.length) * 1;
    var str2 = str.substring(0, 1);
    if (str2 == "s") {
        return str1 * 1000;
    }
    else if (str2 == "h") {
        return str1 * 60 * 60 * 1000;
    }
    else if (str2 == "d") {
        return str1 * 24 * 60 * 60 * 1000;
    }
}

//�������趨����ʱ���ʹ��ʾ���� 
//s20�Ǵ���20�� 
//h��ָСʱ����12Сʱ���ǣ�h12 
//d��������30����d30 

//setCookie("name", "hayden", "s20");

function json_encode(str)
{
    //str = str.replace(/@/g, "@@");
    //str = str.replace(/"/g, "@1");
    //str = str.replace(/'/g, "@2");
    //str = str.replace(/ /g, "@3");
    str = encodeURIComponent(str);
    //alert(str);
    //str = str.replace(/\+/g, "%20");
    //alert(str);
   // alert("encode"+str);
    return str;
}

function json_decode(str) {

    str = decodeURIComponent(str);
    //str = unescape(str);
    //str=str.replace(/@@/g, "@");
    //str=str.replace(/@1/g, '\"');
    //str = str.replace(/@2/g, "\'");
    //str = str.replace(/@3/g, " ");
 //   alert("decode"+str);
    return str;
}

function time_to_string(time)
{
    
    var t = parseInt(time) * 1000;
    if (t == 0)
        return "";
    var date = new Date(t);
    return date.getFullYear()+" �� "+(date.getMonth()+1)+" �� " +date.getDate() +" ��  "+date.getHours()+":"+date.getMinutes()+":"+date.getSeconds();
}
//ת��ת���
//
function escape_decode(str) {
    str = str.replace(/\n/g, "<br/>");
    str = str.replace(/ /g, "&nbsp;");
    //   alert("decode"+str);
    return str;
}
//״̬����
function new_status(lv) {
    if (lv == 1) {
        return "����";
    }
    else if (lv == 2) {
        return "�༭";
    }
    else if (lv == 3) {
        return "����";
    }
    else if (lv == 0) {
        return "ʧЧ";
    }
    return "δ֪";
}
function session_status(lv) {
    if (lv == 1)
    {
        return "��Ծ";
    }
    else if (lv == 0) {
        return "ʧЧ";
    }
    return "δ֪";
}
function user_lv(lv) {
    if (lv == 0) {
        return "����Ա";
    }
    else if (lv == 1) {
        return "�༭Ա";
    }
    else if (lv == 2) {
        return "��ͨ�û�";
    }
    else if (lv == 3) {
        return "��ʱ�û�";
    }
    return "��Ч�˻�";
}

function Error_Code(code)
{
    if (code == 0)
        return "�ɹ�";
    else if (code == -1000)
        return "Ȩ�޲���";
    else if (code == -1001)
        return "�����������";
    else if (code == -1002)
        return "���ݲ�������";
    else if (code == -1003)
        return "ÿҳ��Ϣ���� ����С�ڵ���0";
    else if (code == -1004)
        return "�û�����������Ϊ��";
    else if (code == -1005)
        return "�û����ظ�";
    else if (code == -1006)
        return "�������";
    else if (code == -1007)
        return "����Ա�������";
    else if (code == -1008)
        return "�������޸�admin�û���Ϣ";
    else if (code == 1)
        return "�Ự�ѹ��ڣ������µ�¼";
    else
        return "δ֪����:" + code;

}
//������
function check_str(reg,str)
{
    if (str.length != 0)
    {
        if (reg.test(str))
        {
            ////�뽫�����ڡ��ĳ�����Ҫ��֤����������!    
            return 0
        }
    }
    return -1;
}
function check_username(username)
{
    reg = /^[\u4e00-\u9fa5A-Za-z0-9-_]{3,20}$/;
    if (check_str(reg, username) == -1)
    {
        alert("������û�������ȷ! ֻ����Ӣ�ģ����֣��»��ߣ����š� ������3-20֮��");
        return -3;
    }
    return 0;
}

function check_password(password) {
    reg = /^[a-zA-Z\w]{4,10}$/;
    if (check_str(reg, password) == -1) {
        alert("��������벻��ȷ! ֻ����Ӣ�ģ����֣��»��ߣ����š�������4-10֮��");
        return -2;
    }
    return 0;
}

function check_author(author) {
    reg = /^[\u4e00-\u9fa5A-Za-z0-9-_ ]{3,20}$/;
    if (check_str(reg, author) == -1) {
        alert("���������������ȷ! ֻ����Ӣ�ģ����֣��»��ߣ����š� ������3-20֮��");
        return -2;
    }
    return 0;
}

function check_title(title) {
    //reg = /^[\u4e00-\u9fa5A-Za-z0-9-_]{1,200}$/;
    if (title.length < 1 || title.length >200) {
        alert("����ı��ⲻ��ȷ!������1-200֮��");
        return -2;
    }
    return 0;
}

function check_comment(comment) {
    //reg = /^[\u4e00-\u9fa5A-Za-z0-9-_]{0,600}$/;
    if (comment.length < 1 || comment.length >200) 
    {
        alert("��������۲���ȷ! ֻ����Ӣ�ģ����֣��»��ߣ����š� ������0-600֮��");
        return -2;
    }
    return 0;
}

function text_show(str,size)
{
    if (str.length > size) {
        return str.substring(0, size) + "...";
    }
    else
        return str;
}

function ecode_json_value(str) {
    str = str.replace(/;/g, "��");
    str = str.replace(/&/g, "&amp;");
    str = str.replace(/"/g, "&quot;");
    str = str.replace(/'/g, "&acute;");
    str = str.replace(/</g, "&lt;");
    str = str.replace(/>/g, "&gt;");
    str = str.replace(/\\/g, "&bsol;");
    str = str.replace(/%/g, "&percnt;");//&percnt;
    return str;
}

function dcode_json_value(str) {
    str = str.replace(/&quot;/g, "\"");
    str = str.replace(/&acute;/g, "'");
    str = str.replace(/&lt;/g, "<");
    str = str.replace(/&gt;/g, ">");
    str = str.replace(/&amp;/g, "&");
    str = str.replace(/&bsol;/g, "\\");
    str = str.replace(/&percnt;/g, "%");
   // str = str.replace(/��/g, ";");
    return str;
}