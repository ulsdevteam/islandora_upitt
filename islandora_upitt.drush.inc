<?php

/**
 * @file
 * Drush command/hook implementation for updating existing video objects.
 */

/**
 * Implements hook_drush_command().
 */
function islandora_upitt_drush_command() {
  return array(
    'pitt-fix-page' => array(
      'description' => dt('Fix RELS-INT page dims.'),
      'drupal dependencies' => array(
        'islandora',
        'islandora_paged_content'
      ),
      'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN,
    ),
    'datastream-versionable-flags' => array(
      'aliases' => array('dsv'),
      'description' => dt('Set "versionable" flag on datastreams.'),
      'drupal dependencies' => array(
        'islandora',
      ),
      'options' => array(
        'howmany' => 'How many PID values to process.  Defaults to 1000.',
        'dsids' => 'Datastream Id to process (comma separated values)',
        'set_value' => 'Set the versionable flag to TRUE or FALSE.  Defaults to TRUE.',
      ),
      'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN,
      'examples' => array(
        'Update 5000 OBJ datastreams as versionable' => 'drush -u 1 dsv --howmany=5000 --dsids=OBJ --uri=http://site.library.pitt.edu/',
        'Update 1000 RELS-INT,RELS-EXT,DC datastreams as versionable' =>  'drush -u 1 dsv --dsids=RELS-INT,RELS-EXT,DC --uri=http://site.library.pitt.edu/',
        'Update 1000 MODS datastreams as not versionable' =>  'drush -u 1 dsv --dsids=MODS --set_value=FALSE --uri=http://site.library.pitt.edu/',
      )
    ),
    'paged-content-generate-ocr-datastreams' => array(
      'aliases' => array('pcgod'),
      'description' => dt('Generates the OCR/HOCR datastreams given the options.'),
      'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN,
      'drupal dependencies' => array(
        'islandora_ocr', 'islandora_paged_content'
      ),
      'arguments' => array(
        'pid' => 'The PID value for the book.',
      ),
      'options' => array(
        'aggregate_ocr' => 'Aggregate OCR to the Parent.',
      ),
      'examples' => array(
        'Trigger OCR/HOCR derivatives on islandora:372 and aggregate ocr to the book' => 'drush -u 1 pcgod islandora:372 --aggregate_ocr=1 --uri=http://site.library.pitt.edu/',
        'Trigger OCR/HOCR derivatives on islandora:372' => 'drush -u 1 pcgod islandora:372 --uri=http://site.library.pitt.edu/',
      ),
    ),
    'paged-content-update-pagesize-and-gen-ocr' => array(
      'aliases' => array('pcupago'),
      'description' => dt('Update page image size and generate the OCR/HOCR datastreams given the options.'),
      'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN,
      'drupal dependencies' => array(
        'islandora_ocr', 'islandora_paged_content'
      ),
      'options' => array(
        'pid' => 'The PID value for the book.',
        'aggregate_ocr' => 'Aggregate OCR to the Parent.',
        'process_book_pids_file' => 'Optional file containing PID values on each line to process.',
        'processed_book_pids_file' => 'File to save PID values as they are processed.',
      ),
      'examples' => array(
        'Recalc page sizes and trigger OCR/HOCR derivatives on islandora:372 and aggregate ocr to the book' => 'drush -u 1 pcupago --pid=islandora:372 --aggregate_ocr=1',
        'Recalc page sizes and trigger OCR/HOCR derivatives on islandora:372' => 'drush -u 1 pcupago --pid=islandora:372',
        'Recalc page sizes and trigger OCR/HOCR derivatives processing by file of pid values' => 'drush -u 1 pcupago --process_book_pids_file=/home/bgilling/book_pids.txt --processed_book_pids_file=/home/bgilling/processed_book_pids.txt',
      ),
    )
  );
}

/**
 * Command callback to update citations with PDF derivatives.
 */
function drush_islandora_upitt_pitt_fix_page() {
  module_load_include('inc', 'islandora_paged_content', 'includes/utilities');
  $object = islandora_object_load('pitt:31735051653750-0002');
  drush_print('do');
  islandora_paged_content_add_dimensions_relationships($object, TRUE);
}

/**
 * Command callback to update datastreams versionable flags.  This is an ongoing process to correct 
 * all objects in the system, but will run a Solr query to determine a set of objects that need the update.
 * This set of objects is unordered and just qualifies as having the opposite of set_value set for the 
 * specific datastream ids.
 */
function drush_islandora_upitt_datastream_versionable_flags() {
  module_load_include('inc', 'islandora_upitt', 'includes/drush_helpers');
  $dsids_str = drush_get_option('dsids', '');
  if ($dsids_str) {
    $time_start = microtime_float();

    $dsids = explode(",", $dsids_str);
    foreach ($dsids as $i => $dsid) {
      $dsids[$i] = strtoupper(trim($dsid));
    }

    $howmany = drush_get_option('howmany', 1000);
    $set_value = drush_get_option('set_value', TRUE);
    if (strtoupper($set_value) == 'FALSE') { $set_value = FALSE; }

    $objects = _get_pids($howmany, $dsids, $set_value);
    if (count($objects) > 0) {
      $total_updates = update_datastreams_versionable($objects, $dsids, $set_value);
      echo $total_updates . ' updates on ' . $howmany . ' objects' . "\n";

      $time_end = microtime_float();
      $time = $time_end - $time_start;
      echo $time . ' total seconds' . "\n";
      if ($total_updates > 0) {
        echo $time / $total_updates . ' seconds per update' . "\n";
      }
    }
    else {
      error_log('There are no more objects with a datastream that needs to be updated for the versionable flag.  It is now safe to turn off the cronjob that is calling drush dsv');
    }
  }
}

/**
 * Will generate ocr datastreams.
 *
 * @param string $pid
 *   The PID for the book or page object for which the derivatives will be generated.
 *
 * Options are not defined in the function declaration, but can be derived using drush_get_option() calls
 * @param array $options
 *   The options for tesseract/gimp.
 */
function drush_islandora_upitt_paged_content_generate_ocr_datastreams($pid) {
  $batch =& batch_get();

  module_load_include('inc', 'islandora_paged_content', 'includes/utilities');
  module_load_include('inc', 'islandora_paged_content', 'includes/batch');
  module_load_include('inc', 'islandora_upitt', 'includes/drush_helpers');
  $time_start = microtime_float();

  $islandora_object = islandora_object_load($pid);
  if (!is_object($islandora_object) || (get_class($islandora_object) <> 'IslandoraFedoraObject')){
    drush_set_error(dt('Could not load object for PID:').$pid);
    return false;
  }
  else {
    $aggregate_ocr = drush_get_option('aggregate_ocr', 0);
    $pages = array_keys(islandora_paged_content_get_pages($islandora_object));
    $page_count = count($pages);
    $options = array(
      'language' => 'eng',
      'preprocess' => FALSE,
      'aggregate_ocr' => $aggregate_ocr,
    );
    $batch = islandora_paged_content_create_ocr_batch($islandora_object, $pages, $options);
    batch_set($batch);
    $batch =& batch_get();

    //Because we are doing this on the back-end, we set progressive to false.
    $batch['progressive'] = FALSE;

    drush_backend_batch_process();
  }
  $time_end = microtime_float();
  $time = $time_end - $time_start;
  error_log(number_format($page_count) . ' pages processed in ' . $time);
  if ($page_count > 0) {
    error_log($time / $page_count . ' seconds per page');
  }
  return TRUE;
}

/**
 * This will calculate the width and height of the page's underlying image and set that value
 * in the RELS_INT.
 *
 * @param string $pid
 *   The PID for the book or page object for which the derivatives will be generated.
 */
function drush_islandora_upitt_paged_content_update_page_image_sizes($pid) {
  module_load_include('inc', 'islandora_paged_content', 'includes/utilities');
  module_load_include('inc', 'islandora_upitt', 'includes/drush_helpers');
  $time_start = microtime_float();

  $islandora_object = islandora_object_load($pid);

  if (!is_object($islandora_object)) {
    drush_set_error(dt('Could not load object for PID:').$pid);
  }
  else {
    $pages = array_keys(islandora_paged_content_get_pages($islandora_object));
    $page_count = count($pages);

    error_log('Start updating page size for book ' . $pid);
    // For each PageObject update the height/width of the image in RELS-INT.
    foreach ($pages as $page) {
      # Working on this PID.
      error_log("PID: $page");

      # Get the PageObject.
      $pageObject = islandora_object_load($page);

      if (!is_object($pageObject) || (get_class($pageObject) <> 'IslandoraFedoraObject')){
        drush_set_error(dt('Could not load page object for PID:').$page);
        return false;
      }
      else {
        # Get the RELS-INT size of the image.
        # This should probably be the OBJ datastream but in some cases
        # the OBJ datastream doesn't work.
        $jp2_datastream = $pageObject['JP2'];
        $rels = $jp2_datastream->relationships;
        $width_rels_in = $rels->get(ISLANDORA_RELS_EXT_URI, 'width');
        $width_rels = $width_rels_in[0]['object']['value'];
        $height_rels_in = $rels->get(ISLANDORA_RELS_EXT_URI, 'height');
        $height_rels = $height_rels_in[0]['object']['value'];

        # Get the actual image dimensions.
        $DSID = "OBJ";
        $obj_datastream = $pageObject['OBJ'];
        $mimetype = $obj_datastream->mimetype;
        $fileextension = getMimetypeToFileExtension($mimetype);
        $filename = tempnam("temporary://", "$DSID.$fileextension");
        $retval = getDatastreamToFile($pageObject,"OBJ","$filename");
        list($width, $height) = getimagesize($filename);
        @unlink($filename);

        # Determine if update is needed.
        if (($width == $width_rels) && ($height == $height_rels)) {
        }
        else {
          # Size is incorrect - needs updating.
          set_relationship($rels, ISLANDORA_RELS_EXT_URI, 'width', $width, 1);
          set_relationship($rels, ISLANDORA_RELS_EXT_URI, 'height', $height, 1);

          # Report on the differences.
          error_log("Actual Width: $width, Height: $height,  \n" .
                    "RELS-INT Width: $width_rels, Height: $height_rels");
        }
      }
    }
  }
  $time_end = microtime_float();
  $time = $time_end - $time_start;
  error_log(number_format($page_count) . ' pages processed in ' . $time);
  error_log($time / $page_count . ' seconds per page');
  return TRUE;
}

/**
 * This will basically process two separate drush functions and can optionally work off of a file
 * that contains pid values.  If the options process_book_pids_file and processed_book_pids_file, then
 * the code will attempt to process all of the entries that are contained in the process_book_pids_file
 * but not yet added to the processed_book_pids_file -- this way it can process each entry only once
 * even if it is interrupted.
 *
 * If there are any errors processing the page sizes or OCR for a given PID, that entry will be saved
 * to a file that is saved along side the incoming $process_book_pids_file, but with the filename
 * including "_errors".  If the filename has a file extension, the "_errors" is inserted before the [dot],
 * as in "books.txt" would have a "books_errors.txt"... else when the incoming file did not have a [dot],
 * for example with an incoming file named "books", any errors would be stored in "books_errors".
 *
 * @param string $pid
 *   The PID for the book or page object that will be processed.
 */
function drush_islandora_upitt_paged_content_update_pagesize_and_gen_ocr() {
  module_load_include('inc', 'islandora_upitt', 'includes/drush_helpers');
  islandora_upitt_exit_if_running('drush_pcupago');

  $process_book_pids_file = drush_get_option('process_book_pids_file', '');
  $processed_book_pids_file = drush_get_option('processed_book_pids_file', '');
  $pid = drush_get_option('pid', '');

  if ($process_book_pids_file && $processed_book_pids_file && file_exists($process_book_pids_file)) {
    $file = file($process_book_pids_file);
    $done_file = (file_exists($processed_book_pids_file) ? file($processed_book_pids_file) : array());

    // Get only the PID values that have not been done yet.
    $pids = array_diff($file, $done_file);
  }
  elseif ($pid) {
    $pids[] = $pid;
  }
  foreach ($pids as $pid) {
    $pid = trim($pid);
    if ($pid) {
      error_log('update_pagesize_and_gen_ocr for ' . $pid . "\n---------------------------------------------------" . date('H:i:s'));
      $recalc_pagesizes = drush_islandora_upitt_paged_content_update_page_image_sizes($pid);
      $gen_ocrs = drush_islandora_upitt_paged_content_generate_ocr_datastreams($pid);
      // In a perfect world, this file would only be updated during the batch_finished callback for the "generate_ocr" function call, but
      // but that function call is not provided with the drush parameters that were passed into this call.

      if ($processed_book_pids_file && $recalc_pagesizes && $gen_ocrs) {
        file_put_contents($processed_book_pids_file, $pid . "\n", FILE_APPEND);
      } elseif (!$recalc_pagesizes || !$gen_ocrs) {
        $errors_file = str_replace('.', '_errors.', $processed_book_pids_file);
        $errors_file = (strstr($errors_file, "_errors.")) ? $errors_file : $errors_file . '_errors';
        $pid_and_errors = array($pid);
        if (!$recalc_pagesizes) { $pid_and_errors[] = 'update_page_image_sizes'; }
        if (!$gen_ocrs) { $pid_and_errors[] = 'generate_ocr_datastreams'; }
        file_put_contents($errors_file, implode(",", $pid_and_errors) . "\n", FILE_APPEND);
      }
    }
  }

  $clear_tmp_command = 'rm -rf /ingest/tmp/pitt*';
  exec($clear_tmp_command);
}

