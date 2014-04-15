<?php

require_once dirname(__FILE__) . '/RestClient.inc';

class SynologyBaseAPI
{
    protected $_ws_client;
    protected $_api_info;
    protected $_session_name;
    protected $_sid;

    public function __construct($host = null, $port = null)
    { //{{{
        if (empty($base_url)) {
            $host = "http://roylo.myds.me";
        }
        if (empty($port)) {
            $port = 3565;
        }
        $opt = array(
                  "base_url" => "$host:$port",
                  "format"   => "json",
               );
        $this->_ws_client = new RestClient($opt);
        $this->_ws_client->register_decoder('json',
            create_function('$json_arr', "return json_decode(\$json_arr, TRUE);"));

        $this->getApiAuthInfo();
    } //}}}

    public function __destruct()
    { //{{{
        $this->_doAuth($this->_session_name, "login");
    } //}}}

    public function login($account, $passwd, $session_name, $cookie_file)
    { //{{{
        $curl_opt = array(
            CURLOPT_COOKIEJAR => $cookie_file,
            CURLOPT_COOKIEFILE => $cookie_file,
        );
        $this->_ws_client->set_option("curl_options", $curl_opt);
        $ret = $this->_doAuth($session_name, "login", $account, $passwd);
        if ($ret['success']) {
            $this->_session_name = $session_name;
            $this->_sid = $ret['data']['sid'];
        }
        return $ret['success'];
    } //}}}

    public function logout($session_name)
    { //{{{
        return $this->_doAuth($session_name, "logout");
    } //}}}

    public function getApiAuthInfo()
    { //{{{
        $api = "SYNO.API.Info";
        $version = 1;
        $req = $this->_init_req($api, $version, "query");
        $req["query"] = "all";

        $result = $this->_ws_client->get("webapi/query.cgi", $req);
        $this->_api_info = $result['data'];
        return $result;
    } //}}}

    private function _doAuth($session_name, $method, $account = null, $passwd = null)
    { //{{{
        $api_name = "SYNO.API.Auth";
        $this->_get_basic_info($api_name, $version, $path);

        $req = $this->_init_req($api_name, $version, $method);
        if ($method == "login") {
            $req["account"] = $account;
            $req["passwd"] = $passwd;
            $req["format"] = "cookie";
        }
        $req["session"] = $session_name;
        $result = $this->_ws_client->post("webapi/$path", $req);
        return $result;
    } //}}}

    private function _init_req($api_name, $version, $method)
    { //{{{
        $req = array(
            "api" => $api_name,
            "version"  => $version,
            "method"   => $method,
        );

        return $req;
    } //}}}

    private function _get_basic_info($api_name, &$version, &$path)
    { //{{{
        if (isset($this->_api_info[$api_name])) {
            $version = $this->_api_info[$api_name]['maxVersion'];
            $path = $this->_api_info[$api_name]['path'];
        } else {
            throw new exception("missing API info");
        }
    } //}}}
}


