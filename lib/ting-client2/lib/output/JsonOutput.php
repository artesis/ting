<?php

class JsonOutput implements OutputInterface {

  private $data;

  public function __construct($json) {
    if (is_array($json)) {
      $this->data = $json;
    }
    else {
      $this->data = json_decode($json, true);
    }
  }

  public function getValue($key, $debug = false) {
    $key = explode('/', $key);
    $result = $this->data;
    foreach ($key as $k) {
      if (empty($result[$k])) {
        // Cannot find required data.
        return null;
      }
      $result = $result[$k];
    }

    if (!empty($result['$'])) {
      return $result['$'];
    }

    if (!empty($result[0])) {
      $return = array();
      foreach ($result as $res) {
        $return[] = new JsonOutput($res);
      }
      return $return;
    }

    return new JsonOutput($result);
  }

  public function toArray() {
    $return = array();
    foreach ($this->data as $prefix => $data) {
      if (is_array($data)) {
        foreach ($data as $row) {
          $itemPrefix = null;
          $value = $row['$'];
          if (!empty($row['@type'])) {
            $itemPrefix = explode(':', $row['@type']['$']);
            $itemPrefix = array_pop($itemPrefix);
            $itemPrefix = str_replace('-', '', $itemPrefix);
          }
          $key = $prefix . (($itemPrefix) ? '_' . $itemPrefix : null);
          $key = strtolower($key);
          if (!empty($return[$key]) && is_array($return[$key])) {
            $return[$key][] = $value;
          }
          elseif (!empty($return[$key])) {
            $return[$key] = array($return[$key]);
            $return[$key][] = $value;
          }
          else {
            $return[$key] = $value;
          }
        }
      }
    }
    return $return;
  }

  public function getData() {
    return $this->data;
  }
}
