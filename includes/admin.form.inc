<?php

/**
 * @file
 * Handles the display/submission of the admin settings form for this module.
 */

/**
 * Defines the admin settings form.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 *
 * @return array
 *   The Drupal form definition.
 */
function islandora_upitt_admin_settings(array $form, array &$form_state) {
  global $base_url;
  module_load_include('inc', 'islandora_upitt', 'includes/solr');
  $selectable_datastreams = islandora_upitt_selectable_datastreams('fedora_datastreams_ms', TRUE);
  $form = array(
    'options_0' => array(
      '#type' => 'fieldset',
      '#title' => t('Displaying Datastream Info for Islandora Object "Item Description" page.'),
      'display_dsids' => array(
        '#type' => 'select',
  	'#multiple' => TRUE,
        '#size' => 10,
        '#options' => $selectable_datastreams,
        '#title' => t('Display File Details for Datastream IDs'),
        '#description' => t('Select the datastream ID values that should be displayed in the "Resources" section on an Islandora ' .
          'overview page.'),
        '#default_value' => variable_get('islandora_upitt_display_dsids', array(
            'DC','DESC','EAD','FULL_TEXT','HOCR','INDEXTRANSCRIPT','MARC',
            'MARCXML','MODS','JPG','JP2','OCR','MP4','PDF','PREVIEW',
            'RELS-EXT','RELS-INT','TRANSCRIPT','TN_LARGE','TN')),
      ),
      'view_in_browser_dsids' => array(
        '#type' => 'select',
  	'#multiple' => TRUE,
        '#size' => 10,
        '#options' => $selectable_datastreams,
        '#title' => t('View Datastreams in browser'),
        '#description' => t('List the Datastream IDs for objects that should be viewed in the browser -- all others automatically ' .
          'trigger download.  Depending on the mimetypes that are configured for your browser, some files will always trigger ' .
          'downloads even if the /view method is used.'),
        '#default_value' => variable_get('islandora_upitt_view_in_browser_dsids', array(
            'DC','DESC','EAD','TN','TN_LARGE','JPG','PREVIEW','PDF','MODS','RELS-EXT','RELS-INT')),
      ),
      'possibly_protected_dsids' => array(
        '#type' => 'select',
  	'#multiple' => TRUE,
        '#size' => 10,
        '#options' => $selectable_datastreams,
        '#title' => t('Prevent download based on copyright for Datastream IDs'),
        '#description' => t('From the set of <b>Display File Details for Datastream IDs</b> above, which should be ' .
          'protected from download based on the object\'s copyright and publication status.'),
        '#default_value' => variable_get('islandora_upitt_possibly_protected_dsids', array(
            'EAD','FULL_TEXT','HOCR','JPG','JP2','MP4','PDF','PREVIEW')),
      ),
    ),
    'options_1' => array(
      '#type' => 'fieldset',
      '#title' => t('DNS domain'),
      'dns_domain' => array(
        '#type' => 'textfield',
        '#title' => t('DNS domain name for this site.'),
        '#description' => t('This should be the full domain value for the given site such as "http://www.historicpittsburgh.org/".'),
        '#default_value' => variable_get('islandora_upitt_dns_domain', $base_url),
      ),
    ),
    'options_2' => array(
      '#type' => 'fieldset',
      '#title' => t('Site Jump'),
      'show_sitejump' => array(
        '#type' => 'checkbox',
        '#title' => t('Show the site jump tab.'),
        '#description' => t('Site jump links allow the administrator to quickly navigate to the same islandora ' .
          'content in the other web site.'),
        '#default_value' => variable_get('islandora_upitt_show_sitejump', 1),
      ),
      'sitejump_domain' => array(
        '#type' => 'textfield',
        '#title' => t('Domain for jump'),
        '#description' => t('Domain value for site jumping.'),
        '#default_value' => variable_get('islandora_upitt_sitejump_domain', 'gamera.library.pitt.edu'),
      ),
      'sitejump_tab_text' => array(
        '#type' => 'textfield',
        '#title' => t('Tab Text'),
        '#description' => t('Control the text that appears on the site jump tab.'),
        '#default_value' => variable_get('islandora_upitt_sitejump_tab_text', 'Site Jump'),
      ),
    ),
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Save configuration'),
    ),
    'reset' => array(
      '#type' => 'submit',
      '#value' => t('Reset to defaults'),
    ),
  );
  return $form;
}

/**
 * Function that sets the Drupal variables with user's input.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_upitt_admin_settings_submit(array $form, array &$form_state) {
  drupal_set_message(t('The settings have been updated!'));
  $id = $form_state['triggering_element']['#id'];
  switch ($id) {
    case 'edit-submit':
      variable_set('islandora_upitt_display_dsids', $form_state['values']['display_dsids']);
      variable_set('islandora_upitt_view_in_browser_dsids', $form_state['values']['view_in_browser_dsids']);
      variable_set('islandora_upitt_possibly_protected_dsids', $form_state['values']['possibly_protected_dsids']);
      variable_set('islandora_upitt_dns_domain', $form_state['values']['dns_domain']);
      variable_set('islandora_upitt_show_sitejump', $form_state['values']['show_sitejump']);
      variable_set('islandora_upitt_sitejump_domain', $form_state['values']['sitejump_domain']);
      variable_set('islandora_upitt_sitejump_tab_text', $form_state['values']['sitejump_tab_text']);
      break;

    case 'edit-reset':
      variable_del('islandora_upitt_display_dsids');
      variable_del('islandora_upitt_view_in_browser_dsids');
      variable_del('islandora_upitt_possibly_protected_dsids');
      variable_del('islandora_upitt_dns_domain');
      variable_del('islandora_upitt_show_sitejump');
      variable_del('islandora_upitt_sitejump_domain');
      variable_del('islandora_upitt_sitejump_tab_text');
      break;
  }
}
