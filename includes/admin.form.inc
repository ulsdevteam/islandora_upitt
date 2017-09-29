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

  $form = array(
    'options_0' => array(
      '#type' => 'fieldset',
      '#title' => t('DNS domain'),
      'dns_domain' => array(
        '#type' => 'textfield',
        '#title' => t('DNS domain name for this site.'),
        '#description' => t('This should be the full domain value for the given site such as "http://www.historicpittsburgh.org/".'),
        '#default_value' => variable_get('islandora_upitt_dns_domain', $base_url),
      ),
    ),
    'options_1' => array(
      '#type' => 'fieldset',
      '#title' => t('Site Jump'),
      'show_sitejump' => array(
        '#type' => 'checkbox',
        '#title' => t('Show the site jump tab.'),
        '#description' => t('Site jump links allow the administrator to quickly navigate to the same islandora content in the other web site.'),
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
      variable_set('islandora_upitt_dns_domain', $form_state['values']['dns_domain']);
      variable_set('islandora_upitt_show_sitejump', $form_state['values']['show_sitejump']);
      variable_set('islandora_upitt_sitejump_domain', $form_state['values']['sitejump_domain']);
      variable_set('islandora_upitt_sitejump_tab_text', $form_state['values']['sitejump_tab_text']);
      break;

    case 'edit-reset':
      variable_del('islandora_upitt_dns_domain');
      variable_del('islandora_upitt_show_sitejump');
      variable_del('islandora_upitt_sitejump_domain');
      variable_del('islandora_upitt_sitejump_tab_text');
      break;
  }
}
