<?php

/**
 * Helper function to lookup any solr field value for an islandora_object.
 * 
 * @param mixed $fields
 *   the solr fieldnames to return - could be passed as CSV string.
 * @param boolean $single_result
 *   whether or not to return a single result or array when multistrings are returned (default TRUE)
 * @param string $pid
 *   The Islandora PID value
 *
 * @return string
 *   The value for the field.
 */
function islandora_upitt_get_solr_values($fields, $single_result = TRUE, $pid) {
  $value = NULL;
  if (!is_array($fields)) {
    $fields = explode(",", $fields);
  }
  $query_processor = new IslandoraSolrQueryProcessor();
  $query_processor->solrQuery = 'PID:' . str_replace(":", "\:", $pid);
  $query_processor->solrStart = 0;
  $query_processor->solrLimit = 1;
  $query_processor->solrParams = array(
    'fl' => implode(",", $fields) . ',PID',
    'fq' => '',
  );
  $query_processor->executeQuery(FALSE);
  if ($query_processor->islandoraSolrResult['response']['numFound'] > 0) {
    $solr_results_doc = $query_processor->islandoraSolrResult['response']['objects']['0']['solr_doc'];
    foreach ($fields as $field) {
      if (isset($solr_results_doc[$field])) {
        $value[$solr_results_doc['PID']][$field] = ($single_result && is_array($solr_results_doc[$field])) ?
            $solr_results_doc[$field][0] : implode("<br>", $solr_results_doc[$field]);
      }
      else {
        $value[$solr_results_doc['PID']][$field] = '';
      }
    }
  }
  return $value;
}

/**
 * Helper function that will check for copyright access on a specific datastream.
 *
 * This will use the underlying Islandora object's copyright status value AND the datastream's configuration 
 * to see whether or not any datastream should be restricted from downloads / viewing.
 *
 * @param string $dsid
 *   Datastream ID.
 * @param object $islandora_object
 *   Fedora (Islandora) object.
 * @param array $object_solr_fields
 *   Array of solr fields values of the parent Islandora object (passed by reference - and only updated if 
 *   the values have not been set yet).
 *
 * @return string
 *   The reasons any download is not allowed.
 */
function islandora_upitt_download_prohibit_reason($dsid, $islandora_object, $object_solr_fields) {
  // depending on the object's copyright status and publication status, this could be set to FALSE
  //   mods_accessCondition_copyright_copyright_status_ms and mods_accessCondition_copyright_publication_status_ms
  // also could look at dc.rights and the published date value.
  $possibly_protect_arr = variable_get('islandora_upitt_possibly_protected_dsids');
  if (!array_search($dsid, $possibly_protect_arr) === FALSE) {
    if (!array_key_exists($islandora_object->id, $object_solr_fields)) {
      $fields = array(
        'mods_accessCondition_copyright_copyright_status_ms',
        //  'mods_accessCondition_copyright_publication_status_ms'
      );
      $object_solr_fields = islandora_upitt_get_solr_values($fields, TRUE, $islandora_object->id);
    }

    // Look at the object's copyright status field
    $copyright_status = (array_key_exists('mods_accessCondition_copyright_copyright_status_ms', $object_solr_fields[$islandora_object->id])) ?
       strtolower($object_solr_fields[$islandora_object->id]['mods_accessCondition_copyright_copyright_status_ms']) : '';
/*    $publication_status = (array_key_exists('mods_accessCondition_copyright_publication_status_ms', $object_solr_fields[$islandora_object->id])) ?
*       strtolower($object_solr_fields[$islandora_object->id]['mods_accessCondition_copyright_publication_status_ms']) : '';
*/

    $copyrighted = (strstr($copyright_status, 'copyright') <> '');
    $unknown_copyright = (strstr($copyright_status, 'unknown') <> '');
/*    $copyright_expired = (strstr($copyright_status, 'expired') <> ''); */
/*    $published = ((strstr($publication_status, 'unpublished') == '') && (strstr($publication_status, 'unknown') == '')); */

    $reasons = array();
    if ($copyrighted) {
      $reasons[] = 'Copyrighted';
    }
    if ($unknown_copyright) {
      $reasons[] = 'Unknown copyright status';
    }
/*
* Publication status and published date not important yet - as to consideration for a Public Domain window
*
*    if (!$published) {
*      $reasons[] = 'Not published';
*    }
*/
    return implode(", ", $reasons);
  }
  else {
    return '';
  }
}

/**
 * Gets the Datastream ID values that are currently in the Solr index.
 *
 * @param string $facet_field
 * @param boolean $return_counts
 *   Whether or not the count is returned as part of the value.
 * @return array
 *   An associative array of datastreams depending on $return_counts, the value may
 * include the count of objects that match the datastream.  The key is always
 * the datastream ID.
 */
function islandora_upitt_selectable_datastreams($facet_field, $return_counts = FALSE) {
  return islandora_upitt_facets('fedora_datastreams_ms', $return_counts);
}

/**
 * This will run a Solr query with the base solr filter and return the facets
 * of a specific field.
 *
 * @param type $facet_field
 *   The name of the Solr field to facet upon.
 * @param boolean $return_counts
 *   If true, the value for each datastream will be supplied after the name
 * of the datastream, for example: MODS (484,023)
 * @param integer $facet_limit
 *   How many facet results to return.
 *
 * @return array
 *   An associative array of datastreams depending on $return_counts, the value may
 * include the count of objects that match the datastream.
 */
function islandora_upitt_facets($facet_field, $return_counts = FALSE, $facet_limit = 100) {
  // These datastreams will never be returned:
  $skip_datastreams = array('POLICY', 'COLLECTION_POLICY');

  // Use Solr faceting to get list of names.
  $parsed_url = parse_url(variable_get('islandora_solr_url', 'http://localhost:8080/solr'));
  $solr = new Apache_Solr_Service($parsed_url['host'], $parsed_url['port'], $parsed_url['path']);
  $solr_build = new IslandoraSolrQueryProcessor();
  $query = variable_get('islandora_solr_base_filter', '*:*');
  if (!$query) { $query = '*:*'; }
  $solr_build->solrStart = 0;
  $solr_build->solrLimit = 0;
  $params = array(
    'facet' => 'true',
    'facet.field' => array('fedora_datastreams_ms'),
    'facet.limit' => $facet_limit,
  );
  $solr_build->buildQuery($query, $params);

  $datastreams = array();
  $solr_build->solrParams = array_replace_recursive($solr_build->solrParams, $params);
  try {
    $result_object = json_decode($solr->search($query, 0, 0, $solr_build->solrParams)->getRawResponse());
    $facet_fields = $result_object->facet_counts->facet_fields->$facet_field;
    foreach ($facet_fields as $key => $count) {
      if (array_search($key, $skip_datastreams) === FALSE) {
        $datastreams[$key] = $key . (($return_counts) ? ' (' . number_format($count) . ')' : '');
      }
    }
  }
  catch (Exception $e) {
    $facet_queries = new stdClass();
    drupal_set_message(check_plain(t('Error searching Solr index')) . ' ' . $e->getMessage(), 'error', FALSE);
  }
  ksort($datastreams);
  return $datastreams;
}
