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
//写cookies 

function setCookie(name, value) {
    var Days = 30;
    var exp = new Date();
    exp.setTime(exp.getTime() + Days * 24 * 60 * 60 * 1000);
    document.cookie = name + "=" + escape(value) + ";expires=" + exp.toGMTString();
}

//读取cookies 
function getCookie(name) {
    var arr, reg = new RegExp("(^| )" + name + "=([^;]*)(;|$)");

    if (arr = document.cookie.match(reg))

        return unescape(arr[2]);
    else
        return null;
}

//删除cookies 
function delCookie(name) {
    var exp = new Date();
    exp.setTime(exp.getTime() - 1);
    var cval = getCookie(name);
    if (cval != null)
        document.cookie = name + "=" + cval + ";expires=" + exp.toGMTString();
}
//使用示例 
//setCookie("name", "hayden");
//alert(getCookie("name"));

//如果需要设定自定义过期时间 
//那么把上面的setCookie　函数换成下面两个函数就ok; 


//程序代码 
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

//这是有设定过期时间的使用示例： 
//s20是代表20秒 
//h是指小时，如12小时则是：h12 
//d是天数，30天则：d30 

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
    return date.getFullYear()+" 年 "+(date.getMonth()+1)+" 月 " +date.getDate() +" 日  "+date.getHours()+":"+date.getMinutes()+":"+date.getSeconds();
}
//转换转义符
//
function escape_decode(str) {
    str = str.replace(/\n/g, "<br/>");
    str = str.replace(/ /g, "&nbsp;");
    //   alert("decode"+str);
    return str;
}
//状态解析
function new_status(lv) {
    if (lv == 1) {
        return "创建";
    }
    else if (lv == 2) {
        return "编辑";
    }
    else if (lv == 3) {
        return "发布";
    }
    else if (lv == 0) {
        return "失效";
    }
    return "未知";
}
function session_status(lv) {
    if (lv == 1)
    {
        return "活跃";
    }
    else if (lv == 0) {
        return "失效";
    }
    return "未知";
}
function user_lv(lv) {
    if (lv == 0) {
        return "管理员";
    }
    else if (lv == 1) {
        return "编辑员";
    }
    else if (lv == 2) {
        return "普通用户";
    }
    else if (lv == 3) {
        return "临时用户";
    }
    return "无效账户";
}

function Error_Code(code)
{
    if (code == 0)
        return "成功";
    else if (code == -1000)
        return "权限不足";
    else if (code == -1001)
        return "传入参数错误";
    else if (code == -1002)
        return "数据操作出错";
    else if (code == -1003)
        return "每页信息条数 不可小于等于0";
    else if (code == -1004)
        return "用户名或者密码为空";
    else if (code == -1005)
        return "用户名重复";
    else if (code == -1006)
        return "密码错误";
    else if (code == -1007)
        return "管理员密码错误";
    else if (code == -1008)
        return "不可以修改admin用户信息";
    else if (code == 1)
        return "会话已过期，请重新登录";
    else
        return "未知错误:" + code;

}
//输入检查
function check_str(reg,str)
{
    if (str.length != 0)
    {
        if (reg.test(str))
        {
            ////请将“日期”改成你需要验证的属性名称!    
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
        alert("输入的用户名不正确! 只能中英文，数字，下划线，减号。 长度在3-20之间");
        return -3;
    }
    return 0;
}

function check_password(password) {
    reg = /^[a-zA-Z\w]{4,10}$/;
    if (check_str(reg, password) == -1) {
        alert("输入的密码不正确! 只能中英文，数字，下划线，减号。长度在4-10之间");
        return -2;
    }
    return 0;
}

function check_author(author) {
    reg = /^[\u4e00-\u9fa5A-Za-z0-9-_ ]{3,20}$/;
    if (check_str(reg, author) == -1) {
        alert("输入的作者名不正确! 只能中英文，数字，下划线，减号。 长度在3-20之间");
        return -2;
    }
    return 0;
}

function check_title(title) {
    //reg = /^[\u4e00-\u9fa5A-Za-z0-9-_]{1,200}$/;
    if (title.length < 1 || title.length >200) {
        alert("输入的标题不正确!长度在1-200之间");
        return -2;
    }
    return 0;
}

function check_comment(comment) {
    //reg = /^[\u4e00-\u9fa5A-Za-z0-9-_]{0,600}$/;
    if (comment.length < 1 || comment.length >200) 
    {
        alert("输入的评论不正确! 只能中英文，数字，下划线，减号。 长度在0-600之间");
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
    str = str.replace(/;/g, "；");
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
   // str = str.replace(/；/g, ";");
    return str;
}