<?php

function islandora_upitt_preprocess_islandora_upitt_view(&$variables) {
  dsm($variables, 'var');
  //dsm($variables, 'variables for theme');
  $islandora_object = $variables['islandora_object'];
  $variables['upitt_lg_thumb'] = theme('image', array('path' => url("islandora/object/{$islandora_object->id}/datastream/TN_LARGE/view"), 'title' => $islandora_object->label));

  $mods = $islandora_object['MODS']->content;
  if ($mods) {
    $mods = simplexml_load_string($mods);
    $date = $mods->xpath("/mods:mods/mods:originInfo/mods:dateOther[@type = 'display']/text()");
    $date = (string) $date[0];
    $variables['upitt_date'] = $date;


    $name = $mods->xpath("/mods:mods/mods:name[mods:role/mods:roleTerm = 'depositor']/mods:namePart/text()");
    $name = (string) $name[0];
    $variables['upitt_creator'] = $name;

    module_load_include('inc', 'islandora', 'includes/metadata');
    $variables['metadata'] = islandora_retrieve_metadata_markup($islandora_object);
  }
}

?>