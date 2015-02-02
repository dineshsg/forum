<?php

function multi_step_form($form, &$form_state, $operation = 'add', $folk_id = NULL) {
 
  $form['record_operation'] = array(
      '#type' => 'hidden',
      '#value' => $operation,
  );
 
  $form['report'] = array(
      '#type' => 'markup',
      '#markup' => drupal_render(folks_db_report()),
      '#weight' => 10,
  );
 
  if ($operation == 'modify' || $operation == 'delete') {
 
    if (!isset($form_state['input']['op'])) {
      $db = new DbFolk();
      $folk = $db->get($folk_id);
      $form_state['storage']['first_name'] = $folk->getFirst_name();
      $form_state['storage']['last_name'] = $folk->getLast_name();
      $form_state['storage']['color'] = $folk->getColor();
    }
    $form['id'] = array(
        '#type' => 'hidden',
        '#value' => $folk_id,
    );
  }
 
  switch ($operation) {
    case 'add' :
      $form['record_fieldset'] = array(
          '#type' => 'fieldset',
          '#weight' => 0,
          '#title' => t('Add New Person'),
          '#description' => t('Add a person by
          performing 2 steps. 1) Add the first and last name. 2) Choose the new
          person\'s favorite color.'),
      );
      $submit_value = 'Submit - Complete Form';
      $form = _populate_form_steps($form, $form_state, $submit_value);
      break;
    case 'modify' :
      $form['record_fieldset'] = array(
          '#type' => 'fieldset',
          '#weight' => 0,
          '#title' => t('Edit Record'),
      );
 
      $submit_value = t('Submit Change');
      $form = _populate_form_steps($form, $form_state, $submit_value);
      break;
 
    case 'delete' :
      $form['record_fieldset'] = array(
          '#type' => 'fieldset',
          '#weight' => 0,
          '#title' => t('Delete Record'),
      );
      $submit_value = t('Confirm Delete');
      $additonal_elements = _add_delete_op_elements($form_state, $operation, $folk_id, $submit_value);
      $form = array_merge_recursive($additonal_elements, $form);
      break;
  }
 
  return $form;
}
?>
