<?php

/**
 * @file
 * Drush command/hook implementation for updating existing video objects.
 */

/**
 * Implements hook_drush_command().
 */
function islandora_upitt_drush_command() {
  $commands = array();


  $commands['pitt-fix-page'] = array(
    'description' => dt('Fix RELS-INT page dims.'),
    'drupal dependencies' => array(
      'islandora',
      'islandora_paged_content'
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN,
  );
  return $commands;
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
