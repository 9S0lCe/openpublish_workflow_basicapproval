<?php
/**
 * @file
 * Code for the openpublish Workflow Basic Approval feature.
 */

/**
 * Implements hook_apps_app_info()
 */
function openpublish_workflow_basicapproval_apps_app_info() {
  return array(
    'configure form' => 'openpublish_workflow_basicapproval_configure',
    'status callback' => 'openpublish_workflow_basicapproval_status',
  );
}


/**
 * Configuration form for workflow app
 */
function openpublish_workflow_basicapproval_status() {
  $roles = user_roles();
  // exclude anonymous & admin
  $admin_rid = variable_get('user_admin_role', 0);
  unset($roles[DRUPAL_ANONYMOUS_RID]);
  unset($roles[$admin_rid]);

  $assigned_count = db_select('role_permission', 'p')
    ->condition('module', 'workbench_moderation', '=')
    ->condition('rid', array_keys($roles), 'IN')
    ->countQuery()->execute()->fetchField();

  return array(
    'items' => array(
      'perm' => array(
        'severity' => ($assigned_count == 0) ? REQUIREMENT_WARNING : REQUIREMENT_OK,
        'title' =>  t('Assign moderation permissions'),
        'description' =>  t('To make the most of workflow settings, permissions should be enabled to allow appropriate roles the control to move between workflow states.'),
        'action' => array(l(t('Add default permissions'), 'admin/config/workbench/moderation/transitions/defaults', array('query' => drupal_get_destination()))),
      ),
    ),
  );
}

/**
 * Configuration form for workflow app
 */
function openpublish_workflow_basicapproval_configure($form, $form_state) {
  $form = array();

  $form['enabled'] = array(
    '#type' => 'fieldset',
    '#title' => 'Content Types for Workflow',
  );

  $types = array();
  $types_selected = array();
  foreach (node_type_get_types() as $name => $type) {
    $types[$name] = $type->name;
    if (workbench_moderation_node_type_moderated($name)) {
      $types_selected[$name] = $name;
    }
  }

  $form['enabled']['content_types'] = array(
    '#type' => 'checkboxes',
    '#title' => 'Content types that should have workflow enabled',
    '#options' => $types,
    '#default_value' => $types_selected,
  );
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Save',
  );
  $form = system_settings_form($form); 
  //we want to control our submit
  $form['#submit'] = array('openpublish_workflow_basicapproval_configure_submit');
  return $form;
}

function openpublish_workflow_basicapproval_configure_submit($form, $form_state) {
  foreach ($form_state['values']['content_types'] as $type => $checked) {
    $options = variable_get('node_options_' . $type, array());
    if ($checked) {
      $options[] = 'revision';
      $options[] = 'moderation';
      $options = array_unique($options);
    }
    else {
      $key = array_search('moderation', $options);
      if ($key !== FALSE) {
        unset($options[$key]);
      }
    }
    variable_set('node_options_' . $type, $options);
  }
}
