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
    $variables['upitt_viewer_link'] = l('View this item', $url, array(
      'attributes' => array(
        'class' => array('upitt-viewer-link'))));
    
    $collection_tn_url = '';
    if (isset($islandora_object['TN_LARGE'])) {
      $tn_url = url("islandora/object/{$islandora_object->id}/datastream/TN_LARGE/view");
    } elseif (isset($islandora_object['TN'])) {
      $tn_url = url("islandora/object/{$islandora_object->id}/datastream/TN/view");
    }
    $params = array(
      'title' => $islandora_object->label,
      'alt' => $islandora_object->label,
      'path' => $tn_url);
    $variables['upitt_lg_thumb'] = ($tn_url) ? l(theme('image', $params), $url, array('html' => TRUE)) : '';

    if (isset($islandora_object['MODS'])) {
      // WE no longer need to populate a upitt_date or upitt_creator
      /*
      $mods = $islandora_object['MODS']->content;
      $mods = simplexml_load_string($mods);
      $date = $mods->xpath("/mods:mods/mods:originInfo/mods:dateOther[@type = 'display']/text()");
      $date = isset($date[0]) ? (string) $date[0] : '';
      $variables['upitt_date'] = $date;

      // As of 8/30/16, we no longer want to display the depositor.  BGG
      $name = $mods->xpath("/mods:mods/mods:name[mods:role/mods:roleTerm = 'depositor']/mods:namePart/text()");
      $name = isset($name[0]) ? (string) $name[0] : '';
      // As of 8/30/16, we no longer want to display the depositor.  BGG
      $variables['upitt_creator'] = ''; // $name;
      */

      module_load_include('inc', 'islandora', 'includes/metadata');
      $variables['metadata'] = islandora_retrieve_metadata_markup($islandora_object);
    } else {
      $variables['metadata'] = $variables['upitt_creator'] = $variables['upitt_date'] = '';
    }
  }
}

?>
