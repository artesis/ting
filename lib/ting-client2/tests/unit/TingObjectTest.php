<?php

require_once __DIR__ . '/../../lib/TingObject.php';

class TingObjectTest extends PHPUnit_Framework_TestCase {

  public function createObject(array $data) {
    $jsonOutput = $this->getMock('JsonOutput', array('toArray'));

    $jsonOutput->expects($this->at(0))
      ->method('toArray')
      ->will($this->returnValue($data));

    $jsonOutput->expects($this->at(1))
      ->method('toArray')
      ->will($this->returnValue(array()));

    $stub = $this->getMock('stdClass', array('getValue'));

    $stub->expects($this->at(2))
      ->method('getValue')
      ->with($this->equalTo('record'))
      ->will($this->returnValue($jsonOutput));

    $stub->expects($this->at(5))
      ->method('getValue')
      ->with($this->equalTo('formatsAvailable'))
      ->will($this->returnValue($jsonOutput));

    return new TingObject($stub);
  }

  public function testGetterDefaultValue() {
    $obj = $this->createObject(array('foo' => false, 'bar' => 'zoo', 'duck' => null));

    $this->assertSame(null, $obj->get('non_existing'));

    $this->assertEquals('zoo', $obj->get('bar'));
    $this->assertEquals('zoo', $obj->get('bar', false));
    
    $this->assertSame(false, $obj->get('foo'));
    $this->assertEquals(true, $obj->get('foo', true));

    $this->assertSame(null, $obj->get('duck'));
    $this->assertSame(false, $obj->get('duck', false));
  }
}