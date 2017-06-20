<?php

function accounting_settings_form($form, &$form_state) {
  $form['accounting_emails'] = array(
    '#type' => 'textfield',
    '#title' => t('Enter email recipients.')
  );
}