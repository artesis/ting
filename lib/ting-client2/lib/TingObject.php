<?php
class TingObject {
  protected $data = array();
  protected $relations = array();
  protected $formats = null;

  public function __construct($data) {
    // Get values from collection tag, if it exists.
    $_data = $data->getValue('collection');
    if (!empty($_data)) {
      $data = $_data;
    }

    $_data = $data->getValue('object');
    if (!empty($_data) && is_array($_data)) {
      $data = $_data[0];
    }
    elseif (!empty($_data) && is_a($_data, 'JsonOutput')) {
      $data = $_data;
    }

    $record = $data->getValue('record');

    if (is_a($record, 'JsonOutput')) {
      $record = $record->toArray();
    } else {
      throw new TingClientException('Could not get record');
    }

    $this->data = array_merge($this->data, $record);
    $this->data['objectId'] = $data->getValue('identifier');

    $relations = $data->getValue('relations/relation');

    if (!empty($relations)) {
      foreach ($relations as $row) {
        $relation = array(
          'type' => $row->getValue('relationType'),
          'uri' => $row->getValue('relationUri'),
        );
        $object = $row->getValue('relationObject');
        if (!empty($object)) {
          $relation['object'] = new TingObject($object);
        }
        $this->relations[] = $relation;
      }
    }

    // Get formatsAvailable from response.
    // Must be always an array.
    $formats = $data->getValue('formatsAvailable')->toArray();
    if (!empty($formats['format'])) {
      $formats = $formats['format'];
      if (is_array($formats)) {
        $this->formats = $formats;
      }
      else {
        $this->formats = array($formats);
      }
    }
  }

  public function get($key) {
    if (isset($this->data[$key])) {
      return $this->data[$key];
    }
    return NULL;
  }

  public function getObjectId() {
    return $this->get('objectId');
  }

  public function getRelations() {
    return $this->relations;
  }

  public function getFormats() {
    return $this->formats;
  }

  public function getLocalId() {
    $localId = explode(':', $this->getObjectId());
    return $localId[1];
  }

}
