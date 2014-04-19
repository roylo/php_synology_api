<?php

require_once dirname(__FILE__) . '/lib/SynologyFileStationAPI.inc';
require_once dirname(__FILE__) . '/lib/yaml/axial.configuration.inc';
require_once dirname(__FILE__) . '/lib/yaml/axial.configuration.yaml.inc';
require_once dirname(__FILE__) . '/lib/bizObject/TVShowHandler.inc';


$config = new Axial_Configuration_Yaml('conf/ThunderDispatch.yaml', true);

$login_info = array(
    "account" => $config->account,
    "passwd" => $config->passwd,
    "session_name" => $config->session_name,
    "cookie_file" => $config->cookie_path,
);

$tv_handler = new TVShowHandler(null, $login_info);
$folder_path = "/program/subtitleCron/castle";
$tv_handler->processSubtitleFiles($folder_path);
?>
