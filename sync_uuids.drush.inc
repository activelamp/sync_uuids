<?php

use Symfony\Component\Finder\Finder;
use Symfony\Component\Yaml\Yaml;

function sync_uuids_drush_command() {
    $cmds = [];
    $cmds['sync-uuids'] = [
        'aliases' => ['su'],
        'description' => 'Sync the UUIDs of all active config with the UUIDS found in all staged configs.',
        'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_DATABASE,
        'drupal dependencies' => ['config']
    ];
    return $cmds;
};

function drush_sync_uuids() {

    if (!drush_confirm('Are you sure you want to overwrite the existing UUIDs with the one\'s found in the staged configs?')) {
        return;
    }

    $finder  = new Finder();
    $yamls = $finder->name('*.yml')
        ->in(config_get_config_directory('sync'))
        ->files();

    $configFactory = Drupal::configFactory();

    foreach ($yamls as $file) {
        $key = $file->getBasename('.yml');
        $config = Yaml::parse($file->getRealPath());
        $active = $configFactory->getEditable($key);
        if ($active->get('uuid')) {
            drush_log(sprintf('Updated %s: %s => %s', $key, $active->get('uuid'), $config['uuid']), 'ok');
            $active->set('uuid', $config['uuid'])->save();
        }
    }
}
