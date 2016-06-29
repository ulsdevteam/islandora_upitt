<?php

function islandora_upitt_preprocess_islandora_upitt_view(&$variables) {
  $islandora_object = (isset($variables['islandora_object']) ? $variables['islandora_object'] : NULL);
  if ($islandora_object && isset($islandora_object->id)) {
    $url = url("islandora/object/{$islandora_object->id}/viewer", array(
      'absolute' => TRUE,
      'query' => drupal_get_query_parameters(),

    ));
    //add frag for book object searching and page
    drupal_add_js('jQuery(document).ready(function($) {if(window.location.hash) {
      $(".upitt-viewer-link").attr("href", $(".upitt-viewer-link").attr("href") + window.location.hash);
      }});',
    array('type' => 'inline', 'scope' => 'footer', 'weight' => 5)
  );
    $variables['upitt_viewer_link'] = l(t('View this item'), $url, array(
      'attributes' => array(
        'class' => array('upitt-viewer-link'))));
    $variables['upitt_lg_thumb'] = l(theme('image', array('path' => url("islandora/object/{$islandora_object->id}/datastream/TN_LARGE/view"), 'title' => $islandora_object->label)), $url, array('html' => TRUE));

    $mods = $islandora_object['MODS']->content;
    if ($mods) {
      $mods = simplexml_load_string($mods);
      $date = $mods->xpath("/mods:mods/mods:originInfo/mods:dateOther[@type = 'display']/text()");
      $date = isset($date[0]) ? (string) $date[0] : '';
      $variables['upitt_date'] = $date;


      $name = $mods->xpath("/mods:mods/mods:name[mods:role/mods:roleTerm = 'depositor']/mods:namePart/text()");
      $name = isset($name[0]) ? (string) $name[0] : '';
      $variables['upitt_creator'] = $name;

      module_load_include('inc', 'islandora', 'includes/metadata');
      $variables['metadata'] = islandora_retrieve_metadata_markup($islandora_object);
    }
  }
}

?>
