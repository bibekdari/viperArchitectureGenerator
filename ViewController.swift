//
//  --MODULENAME--ViewController.swift
//
//  Created by VIPER GENERATOR on --TODAY--.
//

import UIKit

class --MODULENAME--ViewController: UIViewController {
    
    // MARK: Properties
    
    var presenter: --MODULENAME--ModuleInterface?
    
    // MARK: IBOutlets
    
    // MARK: VC's Life cycle
    
    override func viewDidLoad() {
        super.viewDidLoad()
        self.setup()
    }
    
    // MARK: IBActions
    
    // MARK: Other Functions
    
    private func setup() {
        // all setup should be done here
    }
}

// MARK: --MODULENAME--ViewInterface
extension --MODULENAME--ViewController: --MODULENAME--ViewInterface {
    
}
