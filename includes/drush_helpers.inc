<?php

/**
 * drush_helpers.inc --- set of functions that are used by the islandora_upitt.drush.inc.  
 * They are grouped here to reduce bulk in that drush unit.
 */

function _get_pids($howmany = 1000, $dsids = array(), $set_value = TRUE) {
  $versionableFilterForDSIDs = array();
  // the filter here is the opposite of the set_value because it needs to find the objects that need to be set to set_value (not to find 
  // the objects that are ALREADY set to set_value.
  $filter_suffix = ':' . (!$set_value ? 'true' : 'false');

  foreach ($dsids as $dsid) {
    $versionableFilterForDSIDs[] = 'fedora_datastream_info_' . $dsid . '_VERSIONABLE_ms' . $filter_suffix ;
  }

  $query_processor = new IslandoraSolrQueryProcessor();

  $query_processor->solrQuery = '(' . implode(' OR ', $versionableFilterForDSIDs) . ')'; 
  $query_processor->solrStart = 0;
  $query_processor->solrLimit = $howmany;
  $query_processor->solrParams = array(
    'fl' => "PID,fgs_label_s",
    'fq' => '',
  );
  $url = parse_url(variable_get('islandora_solr_url', 'localhost:8080/solr'));
  $solr = new Apache_Solr_Service($url['host'], $url['port'], $url['path'] . '/');
  $solr->setCreateDocuments(FALSE);
  try {
    $results = $solr->search($query_processor->solrQuery, $query_processor->solrStart, $query_processor->solrLimit, $query_processor->solrParams, 'GET');
    $tmp = json_decode($results->getRawResponse(), TRUE);
    $results = array();
    foreach ($tmp['response']['docs'] as $trip) {
      $results[$trip['PID']] = array(
          'PID' => $trip['PID'],
          'fgs_label_s' => $trip['fgs_label_s'],
        );
    }
    return $results;
  }
  catch (Exception $e) {
    return array();
  }
}

/**
 * Function to update the datastreams versionable flags for an entire set of object PID values.
 * Returns the total count of object datastream updates to the versionable flag.
 */
function update_datastreams_versionable($objects, $dsids = array(), $set_value = TRUE) {
  $total = 0;
  foreach ($objects as $pid => $object) {
    $total += update_datastream_versionable($pid, $dsids, $set_value);
  }
  return $total;
}

/**
 * See update_datastreams - function to update a single datastream's versionable flag.
 * Returns the number of datastreams that were updated for the given object.
 */
function update_datastream_versionable($pid, $dsids = array(), $set_value = TRUE) {
  $updates = 0;
  $object = islandora_object_load($pid);

  if (!$set_value) { $set_value = FALSE; }
  $dss = array();
  foreach ($dsids as $dsid) {
    $datastream = $object[$dsid];
    if ($datastream->versionable != $set_value) {
      $datastream->versionable = $set_value;
      $dss[] = $dsid;
      $updates++;
    }
    else {
      $dss[] = '(' . $dsid . ' already ' . ($set_value ? 'TRUE' : 'FALSE'). ')';
    }
  }
  return $updates;
}

function getMimetypeToFileExtension($mimetype) {
    $extensions = array(
        'text/xml'              => 'xml',
        'text/html'             => 'html',
        'image/jpeg'            => 'jpg',
        'image/jpg'             => 'jpg',
        'image/tiff'            => 'tif',
        'image/jp2'             => 'jp2',
        'image/png'             => 'png',
        'audio/mpeg'            => 'mp3',
        'application/rdf+xml'   => 'xml',
        'application/xml'       => 'xml',
        'video/mp4'             => 'mp4',
        'video/x-matroska'      => 'mkv',
        'application/pdf'       => 'pdf',
        'audio/x-wav'           => 'wav',
        'text/plain'            => 'txt'
    );
    return $extensions[$mimetype];
}

function getDatastreamToFile($object,$dsid,$file) {
    $datastream = $object["$dsid"];
    if ($datastream) {
        if ( ! is_writable(dirname($file))) {
            print "ERROR: ".dirname($file) . " must be writable!!!\n";
            return(FALSE);
        } else {
            $result = $datastream->getContent($file);
            if (! $result) {
                print "Get datastream failed.\n";
            } else {
                return(TRUE);
            }
        }
    } else {
        print "ERROR: Datastream: ".$dsid." does not exist.\n";
        return(FALSE);
    }
}

function set_relationship($relationships, $predicate_uri, $predicate, $object, $literal = FALSE) {
        $relationships->remove($predicate_uri, $predicate, NULL, $literal);
        $relationships->add($predicate_uri, $predicate, $object, $literal);
}

/**
 * Simple function to replicate PHP 5 behaviour
 */
function microtime_float()
{
    list($usec, $sec) = explode(" ", microtime());
    return ((float)$usec + (float)$sec);
}


