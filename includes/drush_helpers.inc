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
    $versionableFilterForDSIDs[] = '(fedora_datastreams_ms: ' . $dsid . ' AND fedora_datastream_info_' . $dsid . '_VERSIONABLE_ms' . $filter_suffix . ')';
  }

  $query_processor = new IslandoraSolrQueryProcessor();

  // THERE ARE SOME PIDS THAT HAVE NO FEDORA OBJECTS, SO SKIP THESE
  $skip_pids = array('pitt\:31735038311456-0193', 'pitt\:31735051652349-0006', 'pitt\:31735066205257-0145', 'pitt\:00aa57263m-0075', 'pitt\:00acb4439m-0114', 'pitt\:31735066224407-0001', 
                     'pitt\:31735056290988-0013', 'pitt\:31735051653461-0104', 'pitt\:31735051652364-0058', 'pitt\:31735038311563-0207', 'pitt\:00aah1813m-0250');

  $skip_no_object_pids = ' AND -(PID:' . implode(' OR PID:', $skip_pids) . ')';

  $query_processor->solrQuery = '(' . implode(' OR ', $versionableFilterForDSIDs) . ')' . $skip_no_object_pids;

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
  try {
    $object = islandora_object_load($pid);
    if ($object->id) {
      if (!$set_value) { $set_value = FALSE; }
      $dss = array();
      foreach ($dsids as $dsid) {
        if (isset($object[$dsid])) {
          $datastream = $object[$dsid];
          if ($datastream->versionable != $set_value) {
            $datastream->versionable = $set_value;
            $object->ingestDatastream($datastream);
//            $dss[] = $dsid;
            $updates++;
          }
          else {
//            $dss[] = 'for ' . $pid . '["' . $dsid . '"] already ' . ($set_value ? 'TRUE' : 'FALSE'). ')';
          }
        }
        else {
          $dss[] = 'No object["' . $dsid . '"] for PID ' . $pid;
        }
      }
    } else {
      $dss[] = 'NO object for pid ' . $pid;
    }
  }
  catch (Exception $e) {
    error_log('pid not found in fedora : pid = ' . $pid);
  }
  if (count($dss) > 0) {
    error_log(implode("\n", $dss));
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

/**
 * Helper function to save the current process identifier (pid) to the $processID_filename (the specific drush process pid file).
 *
 * NOTE: The /tmp/{drush_PROCESS} files must be writeable on the server.
 */
function islandora_upitt_write_pid_to_tmp($processID_filename) {
  $processID = getmypid();
  $file = fopen($processID_filename,"w+");
  fwrite($file,$processID);
  fclose($file);

  return $processID;
}

/**
 * This code will check for the existence of a pid file for this specific drush process.
 * If that pid file exists, and if there is a process running (from /proc), then this code
 * can exit without further processing.
 */
function islandora_upitt_exit_if_running($processID_filename) {
  $processID_filename = '/tmp/' . str_replace('/tmp/', "", $processID_filename);

  if (file_exists($processID_filename)) {
    $processID_file = file($processID_filename);
    $processID = trim(@$processID_file[0]);
  } else {
    $processID = islandora_upitt_write_pid_to_tmp($processID_filename);
    return;
  }
  // Now, check for this pid value in the running processes:
  $exists = file_exists('/proc/' . $processID);
  if (!$exists) {
    islandora_upitt_write_pid_to_tmp($processID_filename);
  }
  else {
    die("The process is already running - /proc/" . $processID . "\n");
  }
}

