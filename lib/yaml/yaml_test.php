<?php

require_once("./axial.configuration.inc");
require_once("./axial.configuration.yaml.inc");
$config = new Axial_Configuration_Yaml('../../conf/config.yaml',true);
//echo 'file:'. $config->file;
//echo 'Purpose:'. $config->purpose;
print_r($config->data->toArray());
//echo ‘Purpose: ‘.$config->purpose . ‘.<br/>’;
//echo ‘Data: ‘.print_r($config->data->toArray(),true) . ‘.<br/>’;

