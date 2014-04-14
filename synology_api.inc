<?php

require_once dirname(__FILE__) . '/rest_client.inc';

class SynologyAPI
{
    private $_ws_client;
    private $_api_info;

    public function __construct($host = null, $port = null)
    { //{{{
        if (empty($base_url)) {
            $host = "http://roylo.myds.me";
        }
        if (empty($port)) {
            $port = 3565;
        }
        $curl_opt = array(
            CURLOPT_COOKIEJAR => "cookie.txt",
            CURLOPT_COOKIEFILE => "cookie.txt",
        );
        $opt = array(
                  "base_url" => "$host:$port",
                  "format"   => "json",
                  "curl_options" => $curl_opt,
               );
        $this->_ws_client = new RestClient($opt);
        $this->_ws_client->register_decoder('json',
            create_function('$json_arr', "return json_decode(\$json_arr, TRUE);"));

        $this->getApiAuthInfo();
    } //}}}

    public function getApiAuthInfo()
    { //{{{
        $api = "SYNO.API.Info";
        $version = 1;
        $req = $this->_init_req($api, $version, "query");
        $req["query"] = "all";

        $result = $this->_ws_client->get("webapi/query.cgi", $req);
        $this->_api_info = $result['data'];
        print_r($this->_api_info);
        return $result;
    } //}}}

    public function doAuth($session_name, $method)
    { //{{{
        $api_name = "SYNO.API.Auth";
        $this->_get_basic_info($api_name, $version, $path);

        $req = $this->_init_req($api_name, $version, $method);
        if ($method == "login") {
            $req["account"] = "roylo";
            $req["passwd"] = "2aoiurxi";
            $req["format"] = "cookie";
        }
        $req["session"] = $session_name;
        $result = $this->_ws_client->post("webapi/$path", $req);
        print_r($result);
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

$ss = new SynologyAPI();
$res = $ss->doAuth("FileStation", "login");
print_r($res);
//$res = $ss->doAuth("FileStation", "logout");
//print_r($res);

