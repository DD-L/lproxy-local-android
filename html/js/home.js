
$(document).ready(function(){

  // webchannel
  var baseUrl = (/[?&]webChannelBaseUrl=([A-Za-z0-9\-:/\.]+)/.exec(location.search)[1]);
  output("Connecting to WebSocket server at " + baseUrl + ".");
  var socket = new WebSocket(baseUrl);

  // make baseUrl and socket aceessible globally
  window.baseUrl = baseUrl;
  window.socket  = socket;

  socket.onclose = function() {
    $("input#clientState").val('Disconnect');
    var message = "web channel closed";
    $("div#clientStateMessage").html(message);
    console.error(message);
  };
  socket.onerror = function(error) {
    $("input#clientState").val('Disconnect');
    var message = "web channel error: " + error;
    $("div#clientStateMessage").html(message);
    console.error(message);
  };
  socket.onopen = function() {
    output("WebSocket connected, setting up QWebChannel.");
    new QWebChannel(socket, function(channel) {
      // make commun_object object accessible globally
      window.commun_object  = channel.objects.commun_object;

      commun_object.sendStatus.connect(function(message){
        //output('Recevied message: ' + message);
        output(message);
      });
      //commun_object.sendResults.connect(function(message){
      //  $("textarea#results").val(message);
      //});

      commun_object.sendJson.connect(function(json_str, id){
        var obj = JSON.parse(json_str);
        $("div#" + id).find("input#server_name").val(obj.lsslocal.server_name);
        $("div#" + id).find("input#server_port").val(obj.lsslocal.server_port);
        $("div#" + id).find("input#local_port").val(obj.lsslocal.bind_port);
        $("div#" + id).find("input#auth_key").val(obj.lsslocal.auth_key);
      });

      commun_object.offConfig.connect(function(id){
        $("div#" + id).find("select#switch").val('Off').slider("refresh");
      });
      /*
      commun_object.appExit.connect(function(code){
        output('Quit!');
      });
      */

      $("input#clientState").val('Connected');
      commun_object.receiveStatus("Client connected.\n");
      output("Connected to WebChannel, ready to send/receive messages!");
      //$("div#clientStateMessage").html('Connected to WebChannel');

      // read local.json1

      load_json();
    });
  }

  // event


  //$("#exit").click(function(){
  //  commun_object.appQuit();
  //});
  //$("#quit").click(function(){
  //  // test
  //  commun_object.appQuit();
  //});

  $("a#clearStatus").click(function(){
    $("textarea#status").val('');
  });

    $("select#switch").change(function() {
        var ret = $(this).find("option:selected").text();
        // 得到父父节点 id
        var id = $(this).closest('div.config').attr('id');
        if (ret === 'Off' ) {
            commun_object.stop(id);
            return;
        }
        else // 'On'
        {
            // 目前不支持同时开启多个配置
            var size = $("div.config").size();
            for (var _id=1; _id <= size; ++_id) {
                if (Number(_id) === Number(id)) continue;
                if ($("div#" + _id).find('select#switch').find('option:selected').text() === 'On') {
                    $(this).val('Off').slider("refresh");
                    return;
                }
            }

            var $div_id = $("div#" + id);
            var $server_name = $div_id.find("input#server_name").val();
            var $server_port = $div_id.find("input#server_port").val();
            var $local_port  = $div_id.find("input#local_port").val();
            var $auth_key    = $div_id.find("input#auth_key").val();

            if (check_port('server_port', $server_port) === true
                    && check_port('local_port', $local_port) === true) {

                output('id = ' + id);
                output("server_name: " + $server_name);
                output("server_port: " + $server_port);
                output("local_port:  " + $local_port);
                output("auth_key:    " + $auth_key);
                //
                var config = gen_json_str($server_name, $server_port, $local_port, $auth_key);
                //output(config);
                commun_object.run(config, id);
            }
            else {
                $(this).val('Off').slider("refresh");
                //commun_object.stop(id);
                return;

            }
        }
    });
});

function check_port(which, port) {
    if (port < 1 || port > 65535) {
        var message = "Illegal port: " + which + ": " + port;
        output(message);
        alert(message);
        return false;
    }
    return true;
}

function gen_json_str(server_name, server_port, local_port, auth_key) {
    var config = '{\
        "lsslocal":\
        {\
            "bind_addr": "0.0.0.0",\
            "bind_port": ' + local_port + ',\
            "server_name": "' + server_name + '",\
            "server_port": "' + server_port + '",\
            "auth_key": "' + auth_key + '",\
            "zip_on": false,\
            "timeout": 30\
        }\
    }';
    return config;
}

function load_json() {
    var size = $("div.config").size();
    for (var id=1; id <= size; ++id) {
        commun_object.load_json(id);
    }
}

var log_line_counter = function() {};
var $psconsole = $("textarea#status");
function output(message) {
  //$psconsole.append(message + '\n');

  if (log_line_counter.prototype.num === undefined) {
    log_line_counter.prototype.num = 0;
  }

  // 每 20 行清零
  if (++log_line_counter.prototype.num >= 20) {
    log_line_counter.prototype.num = 0;
  }
  else {
    var text = $psconsole.val();
    message = text + message + '\n';
  }
  $psconsole.val(message);
  // Scroll to down in textarea
  if($psconsole.length) {
    $psconsole.scrollTop($psconsole[0].scrollHeight - $psconsole.height());
  }
}

function isKeyEmpty() {
  var key = $("input#keyvalue").val();
  if (!key) {
     var message = "'Secret key' cannot be empty !";
     output('Warning: ' + message);
     alert(message);

     if ($("a#keytextmode").text() === 'plain') {
        $("input#keycipher").focus();
     } else {
        $("input#keyplain").focus();
     }
     return true;
  }
  return false;
}

function getClientStateMessage() {
  var message = $("div#clientStateMessage").html();
  if (message) {
    return message;
  }
  else {
    return 'State Unknown';
  }
}
