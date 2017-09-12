<?php

function islandora_upitt_preprocess_islandora_upitt_view(&$variables) {
  $islandora_object = (isset($variables['islandora_object']) ? $variables['islandora_object'] : NULL);
  if ($islandora_object && isset($islandora_object->id)) {
    drupal_add_html_head_link(array('rel'=>'canonical','href'=>url("islandora/object/{$islandora_object->id}/view")));
    if (isset($islandora_object['PDF'])) {
      $variables['islandora_content'] = l( (isset($variables['islandora_preview_img']) ? $variables['islandora_preview_img'] : 'preview missing'), url("islandora/object/{$islandora_object->id}/datastream/PDF/view", array('absolute' => TRUE)), array('html' => TRUE));
    }

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
    } else {
      $tn_url = '';
    }
    $params = array(
      'title' => $islandora_object->label,
      'alt' => $islandora_object->label,
      'path' => $tn_url);
    $variables['upitt_lg_thumb'] = ($tn_url) ? l(theme('image', $params), $url, array('html' => TRUE)) : '';

    if (isset($islandora_object['MODS'])) {
      $block = module_invoke('upitt_discovery_system_links_block', 'block_view', 'discovery_system_links_block');
      $variables['discovery_links_block'] = render($block['content']);

      module_load_include('module', 'upitt_islandora_solr_search_extras');
      $variables['upitt_date'] = upitt_islandora_solr_search_extras_get_best_date($islandora_object);

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

//      module_load_include('inc', 'islandora', 'includes/metadata');
//      $variables['metadata'] = islandora_retrieve_metadata_markup($islandora_object);
    } else {
//      $variables['metadata'] = 
      $variables['upitt_creator'] = $variables['upitt_date'] = '';
      $variables['discovery_links_block'] = '';
    }
    // Need this javascript to be able to expand / collapse the "Details" fieldset at the bottom
    drupal_add_js('misc/collapse.js');
    drupal_add_js('misc/form.js');
    module_load_include('inc', 'islandora', 'includes/metadata');
    $variables['metadata'] = islandora_retrieve_metadata_markup($islandora_object);
  }
}

?>
